/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 21st, 2018
 */

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "log.h"
#include "queue.h"
#include "crypto.h"

#include "netinterface.h"

bool netinterface_init(void)
{
  netinterface_queue = queue_init();

  log_info("Initialized net interface.");
  return true;
}

bool netinterface_shutdown(void)
{
  queue_free(netinterface_queue);

  log_info("Shutdown net interface.");
  return true;
}

bool has_transport_conn(transport_conn_t *transport_conn)
{
  return queue_get_index(netinterface_queue, transport_conn) != -1;
}

bool has_transport_conn_by_id(int id)
{
  return get_transport_conn_from_id(id) != NULL;
}

bool has_transport_conn_by_keypair(keypair_info_t *keypair_info)
{
  return get_transport_conn_from_keypair(keypair_info) != NULL;
}

transport_conn_t* add_transport_conn(keypair_info_t *keypair_info)
{
  netinterface_next_id++;

  transport_conn_t *transport_conn = malloc(sizeof(transport_conn_t));
  transport_conn->id = netinterface_next_id;
  transport_conn->keypair_info = keypair_info;

  queue_push_right(netinterface_queue, transport_conn);
  return transport_conn;
}

void remove_transport_conn(transport_conn_t *transport_conn)
{
  if (!has_transport_conn(transport_conn))
  {
    return;
  }
  queue_remove_object(netinterface_queue, transport_conn);
  free_transport_conn(transport_conn);
}

void remove_transport_conn_by_id(int id)
{
  transport_conn_t *transport_conn = get_transport_conn_from_id(id);
  if (!transport_conn)
  {
    return;
  }
  remove_transport_conn(transport_conn);
}

transport_conn_t* get_transport_conn_from_id(int id)
{
  for (int i = 0; i <= netinterface_queue->max_index; i++)
  {
    transport_conn_t *transport_conn = queue_get(netinterface_queue, i);
    if (transport_conn->id == id)
    {
      return transport_conn;
    }
  }
  return NULL;
}

transport_conn_t* get_transport_conn_from_keypair(keypair_info_t *keypair_info)
{
  for (int i = 0; i <= netinterface_queue->max_index; i++)
  {
    transport_conn_t *transport_conn = queue_get(netinterface_queue, i);
    if (transport_conn->keypair_info == keypair_info)
    {
      return transport_conn;
    }
  }
  return NULL;
}

void free_transport_conn(transport_conn_t *transport_conn)
{
  transport_conn->id = -1;
  transport_conn->keypair_info = NULL;
  free(transport_conn);
}

void free_transport_conn_by_id(int id)
{
  transport_conn_t *transport_conn = get_transport_conn_from_id(id);
  if (!transport_conn)
  {
    return;
  }
  free_transport_conn(transport_conn);
}
