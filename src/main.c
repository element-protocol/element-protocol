/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "log.h"
#include "util.h"
#include "crypto.h"
#include "task.h"
#include "net.h"
#include "netinterface.h"
#include "p2p.h"
#include "keypairinterface.h"
#include "msginterface.h"
#include "version.h"

typedef enum Argument
{
  CMD_ARG_HELP = 0,
  CMD_ARG_VERSION,

  CMD_ARG_ALLOW_LOCAL_IP,
  CMD_ARG_BIND_ADDRESS,
  CMD_ARG_BIND_PORT,
  CMD_ARG_NO_PORT_MAPPING,
  CMD_ARG_CONNECT,

  CMD_ARG_GEN_KEYPAIR,
  CMD_ARG_IMPORT_KEYPAIR,

  CMD_ARG_UNKNOWN
} argument_t;

typedef struct ArgumentMap
{
  const char *name;
  argument_t type;
  const char *help;
  int num_args;
} argument_map_t;

static argument_map_t arguments_map[] = {
  {"help", CMD_ARG_HELP, "Shows the help information.", 0},
  {"version", CMD_ARG_VERSION, "Shows the version information.", 0},

  {"bind-address", CMD_ARG_BIND_ADDRESS, "<address> Sets the host bind address.", 1},
  {"bind-port", CMD_ARG_BIND_PORT, "<port> Sets the host bind port.", 1},
  {"allow-local-ip", CMD_ARG_ALLOW_LOCAL_IP, "Allow incoming LAN based peer connections.", 0},
  {"disable-port-mapping", CMD_ARG_NO_PORT_MAPPING, "Disables IGD port mapping via miniupnpc.", 0},
  {"connect", CMD_ARG_CONNECT, "<address, port> Attempts to connect to the specified peer.", 2},

  {"generate-keypair", CMD_ARG_GEN_KEYPAIR, "Generates a new cryptographically safe keypair and exports it.", 0},
  {"import-keypair", CMD_ARG_IMPORT_KEYPAIR, "<public_key, private_key, nonce> Imports a keypair and stores it for use later.", 3},
};

static int num_connection_entries = 0;
static connection_entry_t connection_entries[MAX_CONNECTION_ENTRIES];

static int num_keypair_entries = 0;
static keypair_info_t keypair_entries[MAX_KEYPAIR_ENTRIES];

#define NUM_COMMANDS (sizeof(arguments_map) / sizeof(argument_map_t))

argument_t get_argument_from_str(const char *arg)
{
  // verify command argument prefix
  if (!string_startswith(arg, "-") || string_count(arg, "-", true) > 2)
  {
    return CMD_ARG_UNKNOWN;
  }

  // determine the argument type
  for (int i = 0; i < NUM_COMMANDS; i++)
  {
    argument_map_t *argument_map = &arguments_map[i];
    if (string_endswith(arg, argument_map->name))
    {
      return argument_map->type;
    }
  }
  return CMD_ARG_UNKNOWN;
}

argument_map_t* get_argument_map_from_type(argument_t arg_type)
{
  for (int i = 0; i < NUM_COMMANDS; i++)
  {
    argument_map_t *argument_map = &arguments_map[i];
    if (argument_map->type == arg_type)
    {
      return argument_map;
    }
  }
  return NULL;
}

bool parse_arguments(int argc, char **argv)
{
  for (int i = 1; i < argc; i++)
  {
    argument_t arg_type = get_argument_from_str(argv[i]);
    argument_map_t *argument_map = get_argument_map_from_type(arg_type);
    if (!argument_map)
    {
      log_error("Unknown command line argument: <%s>!", argv[i]);
      return false;
    }

    // check to see if the user provided the correct number of
    // arguments required by this option...
    int num_args = (argc - 1) - i;
    if (num_args < argument_map->num_args)
    {
      printf("Usage: -%s, --%s: %s\n", argument_map->name, argument_map->name, argument_map->help);
      return false;
    }
    switch (arg_type)
    {
      case CMD_ARG_HELP:
        printf("Command-line Options:\n");
        for (int i = 0; i < NUM_COMMANDS; i++)
        {
          argument_map_t *argument_map = &arguments_map[i];
          printf("  -%s, --%s: %s\n", argument_map->name, argument_map->name, argument_map->help);
        }
        printf("\n");
        return false;
      case CMD_ARG_VERSION:
        printf("%s v%s-%s\n", APPLICATION_NAME, APPLICATION_VERSION, APPLICATION_RELEASE_NAME);
        return false;
      case CMD_ARG_BIND_ADDRESS:
        i++;
        const char *address = argv[i];
        net_set_bind_address(address);
        break;
      case CMD_ARG_BIND_PORT:
        i++;
        int port = atoi(argv[i]);
        net_set_bind_port(port);
        break;
      case CMD_ARG_ALLOW_LOCAL_IP:
        p2p_set_allow_local_ip(true);
        break;
      case CMD_ARG_NO_PORT_MAPPING:
        net_set_want_port_mapping(false);
        break;
      case CMD_ARG_CONNECT:
        {
          i++;
          const char *address = argv[i];
          i++;
          int port = atoi(argv[i]);

          connection_entry_t connection_entry;
          connection_entry.address = address;
          connection_entry.port = port;

          connection_entries[num_connection_entries] = connection_entry;
          num_connection_entries++;
          break;
        }
      case CMD_ARG_GEN_KEYPAIR:
        {
          keypair_info_t *keypair_info = crypto_generate_keypair();
          crypto_generate_nonce(keypair_info);
          printf("Public Key: %s\n", crypto_export_key(keypair_info->our_public_key));
          printf("Private Key: %s\n", crypto_export_key(keypair_info->our_private_key));
          printf("Nonce: %s\n", crypto_export_key(keypair_info->nonce));
          return false;
        }
      case CMD_ARG_IMPORT_KEYPAIR:
        {
          keypair_info_t *keypair_info = crypto_init_keypair();

          i++;
          const char *encoded_public_key = argv[i];
          i++;
          const char *encoded_private_key = argv[i];
          i++;
          const char *encoded_nonce = argv[i];

          crypto_import_our_keypair(keypair_info, encoded_public_key, encoded_private_key, encoded_nonce);
          keypair_entries[num_keypair_entries] = *keypair_info;
          num_keypair_entries++;
          break;
        }
      default:
        log_error("Unknown command line argument: <%s>!", argv[i]);
        return false;
    }
  }
  return true;
}

void terminate(int sig)
{
  log_info("Shutting down...");
  if (!keypairinterface_shutdown())
  {
    log_error("Failed to shutdown keypair interface!");
    return;
  }
  if (!msginterface_shutdown())
  {
    log_error("Failed to shutdown msg interface!");
    return;
  }
  if (!netinterface_shutdown())
  {
    log_error("Failed to shutdown net interface!");
    return;
  }
  if (!p2p_shutdown())
  {
    log_error("Failed to shutdown p2p!");
    return;
  }
  if (!net_shutdown())
  {
    log_error("Failed to shutdown net!");
    return;
  }
  if (!taskmgr_shutdown())
  {
    log_error("Failed to shutdown taskmgr!");
    return;
  }
  log_info("Core shutdown.");
}

int main(int argc, char **argv)
{
  if (!parse_arguments(argc, argv))
  {
    return 1;
  }
  log_info("Initializing core...");
  if (!taskmgr_init())
  {
    log_error("Failed to initialize taskmgr!");
    return 1;
  }
  if (!net_init(num_connection_entries, connection_entries))
  {
    log_error("Failed to initialize net!");
    return 1;
  }
  if (!p2p_init())
  {
    log_error("Failed to initialize p2p!");
    return 1;
  }
  if (!netinterface_init())
  {
    log_error("Failed to initialize net interface!");
    return 1;
  }
  if (!keypairinterface_init(num_keypair_entries, keypair_entries))
  {
    log_error("Failed to initialize keypair interface!");
    return 1;
  }
  if (!msginterface_init())
  {
    log_error("Failed to initialize msg interface!");
    return 1;
  }
  log_info("Core initialized.");
  signal(SIGINT, terminate);
  taskmgr_run();
  return 0;
}
