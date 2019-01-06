/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 9th, 2018
 */

#pragma once

#include <stdbool.h>
#include <arpa/inet.h>

#include "dyad.h"
#include "crypto.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DEFAULT_SOCKET_TIMEOUT 0.001

#define DEFAULT_LOCAL_ADDRESS "127.0.0.1"
#define DEFAULT_ADDRESS "0.0.0.0"

#define DEFAULT_PORT 5000
#define DEFAULT_BACKLOG 10000

#define PEERLIST_RESYNC_DELAY 15

#define MAX_CONNECTION_ENTRIES 1000

typedef struct ConnectionEntry
{
  const char *address;
  int port;
} connection_entry_t;

typedef struct Connection
{
  dyad_Stream *stream;
  dyad_Stream *remote;
  bool authenticated;
  keypair_info_t *keypair_info;
  bool encrypted;
} connection_t;

bool netbase_get_is_valid_address(const char *address);
bool netbase_get_is_local_address(const char *address);

#ifdef __cplusplus
}
#endif
