/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#pragma once

#include <stdbool.h>
#include <pthread.h>

#include "dyad.h"
#include "queue.h"
#include "buffer.h"
#include "netbase.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Peer
{
  int id;
  const char *address;
  int port;
  connection_t *connection;
} peer_t;

static int p2p_next_peer_id = -1;
static queue_t *p2p_peer_queue;
static pthread_mutex_t p2p_file_mutex = PTHREAD_MUTEX_INITIALIZER;
static const char *peer_filename = "peerlist.bin";
static bool p2p_allow_local_ip = false;

bool p2p_init(void);
bool p2p_shutdown(void);

void p2p_set_allow_local_ip(bool allow_local_ip);
bool p2p_get_allow_local_ip(void);

int get_next_peer_id(void);
int get_num_peers(void);

bool load_peerlist_from_file(const char *filename);
bool save_peerlist_to_file(const char *filename);

bool has_peer(peer_t *peer);
bool has_peer_by_id(int id);
bool has_peer_by_address(const char *address, int port);

peer_t* add_peer(connection_t *connection, const char *address, int port);
void remove_peer(peer_t *peer);
void remove_peer_by_id(int id);
void remove_peer_by_address(const char *address, int port);

peer_t* get_peer_from_id(int id);
peer_t* get_peer_from_address(const char *address, int port);

void free_peer(peer_t *peer);
void free_peer_by_id(int id);

bool serialize_peerlist_to_buffer(buffer_t *buffer);
bool deserialize_peerlist_from_buffer(buffer_t *buffer);

#ifdef __cplusplus
}
#endif
