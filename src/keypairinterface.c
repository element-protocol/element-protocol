/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, December 16th, 2018
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "log.h"
#include "crypto.h"
#include "queue.h"
#include "task.h"

#include "keypairinterface.h"

bool keypairinterface_init(int num_keypair_entries, keypair_info_t keypair_entries[])
{
  keypairinterface_queue = queue_init();

  // initialize keypairs
  for (int i = 0; i < num_keypair_entries; i++)
  {
    keypair_info_t *keypair_info = &keypair_entries[i];
    keypair_storage_t *keypair_storage = add_keypair(keypair_info);
    if (!keypair_storage)
    {
      continue;
    }
  }

  log_info("Initialized keypair interface.");
  return true;
}

bool keypairinterface_shutdown(void)
{
  queue_free(keypairinterface_queue);

  log_info("Shutdown keypair interface.");
  return true;
}

int get_next_keypair_id(void)
{
  return keypairinterface_next_id;
}

int get_num_keypairs(void)
{
  return queue_get_size(keypairinterface_queue);
}

bool has_keypair(keypair_storage_t *keypair_storage)
{
  return queue_get_index(keypairinterface_queue, keypair_storage) != -1;
}

bool has_keypair_by_id(int id)
{
  return get_keypair_from_id(id) != NULL;
}

keypair_storage_t* add_keypair(keypair_info_t *keypair_info)
{
  keypairinterface_next_id++;

  keypair_storage_t *keypair_storage = malloc(sizeof(keypair_storage_t));
  keypair_storage->id = keypairinterface_next_id;
  keypair_storage->keypair_info = keypair_info;

  queue_push_right(keypairinterface_queue, keypair_storage);
  return keypair_storage;
}

void remove_keypair(keypair_storage_t *keypair_storage)
{
  if (!has_keypair(keypair_storage))
  {
    return;
  }
  queue_remove_object(keypairinterface_queue, keypair_storage);
  free_keypair(keypair_storage);
}

void remove_keypair_by_id(int id)
{
  keypair_storage_t *keypair_storage = get_keypair_from_id(id);
  if (!keypair_storage)
  {
    return;
  }
  remove_keypair(keypair_storage);
}

keypair_storage_t* get_keypair_from_id(int id)
{
  for (int i = 0; i <= keypairinterface_queue->max_index; i++)
  {
    keypair_storage_t *keypair_storage = queue_get(keypairinterface_queue, i);
    if (keypair_storage->id == id)
    {
      return keypair_storage;
    }
  }
  return NULL;
}

void free_keypair(keypair_storage_t *keypair_storage)
{
  keypair_storage->id = -1;
  free(keypair_storage);
}

void free_keypair_by_id(int id)
{
  keypair_storage_t *keypair_storage = get_keypair_from_id(id);
  if (!keypair_storage)
  {
    return;
  }
  free_keypair(keypair_storage);
}
