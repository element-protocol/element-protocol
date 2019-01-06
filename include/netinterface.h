/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 21st, 2018
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "queue.h"
#include "crypto.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TransportConn
{
  int id;
  keypair_info_t *keypair_info;
} transport_conn_t;

static int netinterface_next_id = -1;
static queue_t *netinterface_queue;

bool netinterface_init(void);
bool netinterface_shutdown(void);

bool has_transport_conn(transport_conn_t *transport_conn);
bool has_transport_conn_by_id(int id);
bool has_transport_conn_by_keypair(keypair_info_t *keypair_info);

transport_conn_t* add_transport_conn(keypair_info_t *keypair_info);

void remove_transport_conn(transport_conn_t *transport_conn);
void remove_transport_conn_by_id(int id);

transport_conn_t* get_transport_conn_from_id(int id);
transport_conn_t* get_transport_conn_from_keypair(keypair_info_t *keypair_info);

void free_transport_conn(transport_conn_t *transport_conn);
void free_transport_conn_by_id(int id);

#ifdef __cplusplus
}
#endif
