/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, December 16th, 2018
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "queue.h"
#include "task.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_KEYPAIR_ENTRIES 1000

typedef struct KeypairStorage
{
  int id;
  keypair_info_t *keypair_info;
} keypair_storage_t;

static int keypairinterface_next_id = -1;
static queue_t *keypairinterface_queue;

bool keypairinterface_init(int num_keypair_entries, keypair_info_t keypair_entries[]);
bool keypairinterface_shutdown(void);

int get_next_keypair_id(void);
int get_num_keypairs(void);

bool has_keypair(keypair_storage_t *keypair_storage);
bool has_keypair_by_id(int id);

keypair_storage_t* add_keypair(keypair_info_t *keypair_info);

void remove_keypair(keypair_storage_t *keypair_storage);
void remove_keypair_by_id(int id);

keypair_storage_t* get_keypair_from_id(int id);

void free_keypair(keypair_storage_t *keypair_storage);
void free_keypair_by_id(int id);

#ifdef __cplusplus
}
#endif
