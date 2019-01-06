/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdint.h>
#include <stdbool.h>

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>

#include "log.h"
#include "dyad.h"
#include "queue.h"
#include "crypto.h"
#include "netbase.h"
#include "buffer.h"
#include "protocol.h"
#include "version.h"

#include "net.h"

bool net_init(int num_connection_entries, connection_entry_t connection_entries[])
{
  dyad_init();
  dyad_setTickInterval(DEFAULT_SOCKET_TIMEOUT);
  dyad_setUpdateTimeout(DEFAULT_SOCKET_TIMEOUT);

  net_accept_queue = queue_init();
  net_connection_queue = queue_init();

  // initialize the socket
  dyad_Stream *net_stream = dyad_newStream();
  if (!net_open_tcp_server(net_stream, net_bind_address, net_bind_port, net_backlog))
  {
    return false;
  }

  // initialize all connections
  for (int i = 0; i < num_connection_entries; i++)
  {
    connection_entry_t connection_entry = connection_entries[i];
    dyad_Stream *conn_stream = dyad_newStream();
    if (!net_open_tcp_connection(conn_stream, connection_entry.address, connection_entry.port))
    {
      continue;
    }
  }

  net_poll_events_task = add_task(net_poll_events, 0);
  net_poll_resync_task = add_task(net_poll_resync_peers, PEERLIST_RESYNC_DELAY);
  log_info("Initialized net.");
  return true;
}

bool net_shutdown(void)
{
  remove_task(net_poll_events_task);
  remove_task(net_poll_resync_task);

  queue_free(net_accept_queue);
  queue_free(net_connection_queue);

  dyad_shutdown();
  log_info("Shutdown net.");
  return true;
}

void net_set_bind_address(const char *address)
{
  net_bind_address = address;
}

const char* net_get_bind_address(void)
{
  return net_bind_address;
}

void net_set_bind_port(int port)
{
  net_bind_port = port;
}

int net_get_bind_port(void)
{
  return net_bind_port;
}

void net_set_backlog(int backlog)
{
  net_backlog = backlog;
}

int net_get_backlog(void)
{
  return net_backlog;
}

void net_set_want_port_mapping(bool want_port_mapping)
{
  net_want_port_mapping = want_port_mapping;
}

bool net_get_want_port_mapping(void)
{
  return net_want_port_mapping;
}

connection_t* net_init_connection(dyad_Stream *stream, dyad_Stream *remote)
{
  connection_t *connection = malloc(sizeof(connection_t));
  connection->stream = stream;
  connection->remote = remote;
  connection->authenticated = false;
  connection->keypair_info = NULL;
  connection->encrypted = false;

  queue_push_right(net_accept_queue, connection);
  return connection;
}

void net_free_connection(connection_t *connection)
{
  connection->stream = NULL;
  connection->authenticated = false;

  crypto_free_keypair(connection->keypair_info);
  connection->keypair_info = NULL;
  connection->encrypted = false;

  free(connection);
}

void net_setup_portmapping(int port)
{
  log_info("Trying to add IGD port mapping...");
  int result;

#if MINIUPNPC_API_VERSION > 13
  unsigned char ttl = 2;
  struct UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, 0, 0, ttl, &result);
#else
  struct UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, 0, 0, &result);
#endif

  struct UPNPUrls urls;
  struct IGDdatas igdData;
  char lanAddress[64];
  result = UPNP_GetValidIGD(deviceList, &urls, &igdData, lanAddress, sizeof lanAddress);
  freeUPNPDevlist(deviceList);

  if (result > 0)
  {
    if (result == 1)
    {
      char *port_string = malloc(sizeof(port));
      sprintf(port_string, "%d", port);

      UPNP_DeletePortMapping(urls.controlURL, igdData.first.servicetype, port_string, "TCP", 0);
      int portMappingResult = UPNP_AddPortMapping(urls.controlURL, igdData.first.servicetype,
        port_string, port_string, lanAddress, APPLICATION_NAME, "TCP", 0, "0");

      if (portMappingResult != 0)
      {
        log_warning("Failed to add IGD port mapping!");
      }
      else
      {
        log_info("Added IGD port mapping.");
      }
      free(port_string);
    }
    else if (result == 2)
    {
      log_warning("Failed to add IGD port mapping, could not connect IGD port mapping!");
    }
    else if (result == 3)
    {
      log_warning("Failed to add IGD port mapping, UPnP device was not recoginzed as IGD!");
    }
    else
    {
      log_error("Failed to add IGD port mapping, invalid code returned <%d>!", result);
    }
    FreeUPNPUrls(&urls);
  }
  else
  {
    log_warning("Failed to add IGD port mapping, UPnP device was not recoginzed as IGD!");
  }
}

void net_on_connect(dyad_Event *event)
{
  connection_t *connection = event->udata;
  queue_push_right(net_connection_queue, connection);
  handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_CONNECT_REQ);
}

void net_on_data(dyad_Event *event)
{
  connection_t *connection = event->udata;
  buffer_t *buffer = buffer_init_data(0, (const unsigned char*)event->data, event->size);
  uint16_t payload_size = buffer_read_uint16(buffer);
  if (buffer_get_remaining_size(buffer) < payload_size)
  {
    return;
  }

  if (connection->encrypted)
  {
    int raw_payload_size = buffer_read_uint16(buffer);
    char *payload = buffer_read(buffer, payload_size);

    // attempt to decrypt the text, if we fail then disconnect them,
    // otherwise handle the packet as we would normally.
    unsigned char decrypted[raw_payload_size];
    int result = crypto_box_open_easy(decrypted, (unsigned char*)payload, payload_size,
      connection->keypair_info->nonce, connection->keypair_info->our_public_key,
      connection->keypair_info->their_private_key);

    if (result != 0)
    {
      log_error("Failed to decrypt incoming packet data with error code <%d>!", result);
      dyad_close(connection->stream);
    }
    else
    {
      buffer_t *buffer = buffer_init_data(0, (const unsigned char*)decrypted, raw_payload_size);
      handle_incoming_packet(connection, buffer);
    }
    free(payload);
    buffer_free(buffer);
  }
  else
  {
    handle_incoming_packet(connection, buffer);
  }
}

void net_on_close(dyad_Event *event)
{
  const char *address = dyad_getAddress(event->stream);
  int port = dyad_getPort(event->stream);

  // remove the peer from the peerlist if they
  // were present in the peerlist...
  if (has_peer_by_address(address, port))
  {
    remove_peer_by_address(address, port);
  }

  // free the connection reference from memory
  connection_t *connection = event->udata;

  // attempt to remove the connection from one of the queue
  // object's it may be in...
  queue_remove_object(net_accept_queue, connection);
  queue_remove_object(net_connection_queue, connection);

  net_free_connection(connection);
}

void net_on_error(dyad_Event *event)
{
  log_trace("An error has occurred: %s", event->msg);
}

void net_on_accept(dyad_Event *event)
{
  connection_t *connection = net_init_connection(event->stream, event->remote);

  dyad_addListener(event->remote, DYAD_EVENT_DATA, net_on_data, connection);
  dyad_addListener(event->remote, DYAD_EVENT_CLOSE, net_on_close, connection);
}

bool net_open_tcp_server(dyad_Stream *stream, const char *address, int port, size_t backlog)
{
  // check to see if this address is a valid pv4 address...
  if (!netbase_get_is_valid_address(address))
  {
    return false;
  }

  if (net_want_port_mapping)
  {
    net_setup_portmapping(port);
  }

  dyad_addListener(stream, DYAD_EVENT_ERROR, net_on_error, NULL);
  dyad_addListener(stream, DYAD_EVENT_ACCEPT, net_on_accept, NULL);

  dyad_setNoDelay(stream, 1);
  dyad_listenEx(stream, address, port, backlog);
  return true;
}

bool net_open_tcp_connection(dyad_Stream *stream, const char *address, int port)
{
  // check to see if this address is a valid pv4 address...
  if (!netbase_get_is_valid_address(address))
  {
    return false;
  }

  connection_t *connection = net_init_connection(stream, stream);

  dyad_addListener(stream, DYAD_EVENT_ERROR, net_on_error, NULL);
  dyad_addListener(stream, DYAD_EVENT_CONNECT, net_on_connect, connection);
  dyad_addListener(stream, DYAD_EVENT_DATA, net_on_data, connection);
  dyad_addListener(stream, DYAD_EVENT_CLOSE, net_on_close, connection);

  dyad_setNoDelay(stream, 1);
  dyad_connect(stream, address, port);
  return true;
}

task_result_t net_poll_events(task_t *task, va_list args)
{
  dyad_update();
  return TASK_RESULT_CONT;
}

task_result_t net_poll_resync_peers(task_t *task, va_list args)
{
  for (int i = 0; i <= net_connection_queue->max_index; i++)
  {
    connection_t *connection = queue_get(net_connection_queue, i);
    if (!connection)
    {
      continue;
    }

    // ensure this connection has been authenticated before
    // attempting to send peerlist request...
    if (!connection->authenticated)
    {
      continue;
    }
    handle_packet(connection, PKT_DIRECTION_SEND, PKT_TYPE_PEERLIST_REQ);
  }
  return TASK_RESULT_WAIT;
}
