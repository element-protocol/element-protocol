/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdbool.h>
#include <pthread.h>

#include "log.h"
#include "dyad.h"
#include "queue.h"
#include "buffer.h"
#include "netbase.h"
#include "net.h"
#include "util.h"

#include "p2p.h"

bool p2p_init(void)
{
  p2p_peer_queue = queue_init();
  log_info("Initialized p2p.");
  return true;
}

bool p2p_shutdown(void)
{
  queue_free(p2p_peer_queue);
  log_info("Shutdown p2p.");
  return true;
}

void p2p_set_allow_local_ip(bool allow_local_ip)
{
  p2p_allow_local_ip = allow_local_ip;
}

bool p2p_get_allow_local_ip(void)
{
  return p2p_allow_local_ip;
}

int get_next_peer_id(void)
{
  return p2p_next_peer_id;
}

int get_num_peers(void)
{
  return queue_get_size(p2p_peer_queue);
}

bool load_peerlist_from_file(const char *filename)
{
  log_info("Loading peerlist from file: %s...", filename);
  pthread_mutex_lock(&p2p_file_mutex);
  FILE *fp;
  fp = fopen(filename, "r");

  // get the size in bytes of the file on disk
  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // read all of the data from the file into memory
  unsigned char *data = malloc(fsize + 1);
  fread(data, fsize, 1, fp);

  buffer_t *buffer = buffer_init_data(0, data, fsize);
  if (!deserialize_peerlist_from_buffer(buffer))
  {
    log_error("Failed to deserialize peerlist from buffer!");
    return false;
  }
  free(data);
  fclose(fp);
  pthread_mutex_unlock(&p2p_file_mutex);
  log_info("Loaded peerlist.");
  return true;
}

bool save_peerlist_to_file(const char *filename)
{
  log_info("Saving peerlist...");
  pthread_mutex_lock(&p2p_file_mutex);
  FILE *fp;
  fp = fopen(filename, "w");
  buffer_t *buffer = buffer_init();
  if (!serialize_peerlist_to_buffer(buffer))
  {
    log_error("Failed to serialize peerlist to buffer!");
    return false;
  }

  // get the data from the buffer and write it out to the file
  const unsigned char *data = buffer_get_data(buffer);
  fwrite(data, buffer_get_size(buffer), 1, fp);

  fclose(fp);
  pthread_mutex_unlock(&p2p_file_mutex);
  log_info("Saved peerlist.");
  return true;
}

bool has_peer(peer_t *peer)
{
  return queue_get_index(p2p_peer_queue, peer) != -1;
}

bool has_peer_by_id(int id)
{
  return get_peer_from_id(id) != NULL;
}

bool has_peer_by_address(const char *address, int port)
{
  return get_peer_from_address(address, port) != NULL;
}

peer_t* add_peer(connection_t *connection, const char *address, int port)
{
  if (has_peer_by_address(address, port))
  {
    if (p2p_allow_local_ip && netbase_get_is_local_address(address))
    {

    }
    else
    {
      return NULL;
    }
  }

  p2p_next_peer_id++;

  peer_t *peer = malloc(sizeof(peer_t));
  peer->id = p2p_next_peer_id;
  peer->address = address;
  peer->port = port;
  peer->connection = connection;

  queue_push_right(p2p_peer_queue, peer);
  return peer;
}

void remove_peer(peer_t *peer)
{
  if (!has_peer(peer))
  {
    return;
  }
  queue_remove_object(p2p_peer_queue, peer);
  free_peer(peer);
}

void remove_peer_by_id(int id)
{
  peer_t *peer = get_peer_from_id(id);
  if (!peer)
  {
    return;
  }
  remove_peer(peer);
}

void remove_peer_by_address(const char *address, int port)
{
  remove_peer(get_peer_from_address(address, port));
}

peer_t* get_peer_from_id(int id)
{
  for (int i = 0; i <= p2p_peer_queue->max_index; i++)
  {
    peer_t *peer = queue_get(p2p_peer_queue, i);
    if (peer->id == id)
    {
      return peer;
    }
  }
  return NULL;
}

peer_t* get_peer_from_address(const char *address, int port)
{
  for (int i = 0; i <= p2p_peer_queue->max_index; i++)
  {
    peer_t *peer = queue_get(p2p_peer_queue, i);
    if (string_equals(peer->address, address) && peer->port == port)
    {
      return peer;
    }
  }
  return NULL;
}

void free_peer(peer_t *peer)
{
  peer->id = -1;
  peer->address = NULL;
  peer->port = 0;
  peer->connection = NULL;
  free(peer);
}

void free_peer_by_id(int id)
{
  peer_t *peer = get_peer_from_id(id);
  if (!peer)
  {
    return;
  }
  free_peer(peer);
}

bool serialize_peerlist_to_buffer(buffer_t *buffer)
{
  buffer_write_uint16(buffer, get_num_peers());
  for (int i = 0; i <= p2p_next_peer_id; i++)
  {
    peer_t *peer = get_peer_from_id(i);
    if (!peer)
    {
      continue;
    }
    buffer_write_string(buffer, peer->address, strlen(peer->address));
    buffer_write_uint32(buffer, peer->port);
  }
  return true;
}

bool deserialize_peerlist_from_buffer(buffer_t *buffer)
{
  uint16_t num_peers = buffer_read_uint16(buffer);
  for (int i = 1; i <= num_peers; i++)
  {
    char *address = buffer_read_string(buffer);
    uint32_t port = buffer_read_uint32(buffer);

    if (netbase_get_is_local_address(address) && port == net_get_bind_port())
    {
      // no reason to try and connect to ourself!
      continue;
    }

    // try and connect to this potential peer only if we
    // don't already have connection to it...
    if (!has_peer_by_address(address, port))
    {
      dyad_Stream *conn_stream = dyad_newStream();
      if (!net_open_tcp_connection(conn_stream, address, port))
      {
        continue;
      }
    }
  }
  return true;
}
