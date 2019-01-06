/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 20th, 2018
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "queue.h"
#include "task.h"

#include "msginterface.h"

bool msginterface_init(void)
{
  msginterface_queue = queue_init();
  msginterface_poll_task = add_task(poll_msginterface, 0);

  log_info("Initialized msg interface.");
  return true;
}

bool msginterface_shutdown(void)
{
  remove_task(msginterface_poll_task);
  queue_free(msginterface_queue);

  log_info("Shutdown msg interface.");
  return true;
}

bool has_msg(pending_msg_t *pending_msg)
{
  return queue_get_index(msginterface_queue, pending_msg) != -1;
}

bool has_msg_by_id(int id)
{
  return get_msg_from_id(id) != NULL;
}

bool has_msg_by_checksum(const char *checksum)
{
  return get_msg_from_checksum(checksum) != NULL;
}

pending_msg_t* add_msg(const char* checksum, int size, time_t timestamp)
{
  msginterface_next_id++;

  pending_msg_t *pending_msg = malloc(sizeof(pending_msg_t));
  pending_msg->id = msginterface_next_id;
  pending_msg->checksum = checksum;
  pending_msg->size = size;
  pending_msg->timestamp = timestamp;

  queue_push_right(msginterface_queue, pending_msg);
  return pending_msg;
}

void remove_msg(pending_msg_t *pending_msg)
{
  if (!has_msg(pending_msg))
  {
    return;
  }
  queue_remove_object(msginterface_queue, pending_msg);
  free_msg(pending_msg);
}

void remove_msg_by_id(int id)
{
  pending_msg_t *pending_msg = get_msg_from_id(id);
  if (!pending_msg)
  {
    return;
  }
  remove_msg(pending_msg);
}

pending_msg_t* get_msg_from_id(int id)
{
  for (int i = 0; i <= msginterface_queue->max_index; i++)
  {
    pending_msg_t *pending_msg = queue_get(msginterface_queue, i);
    if (pending_msg->id == id)
    {
      return pending_msg;
    }
  }
  return NULL;
}

bool get_msg_has_expired(time_t timestamp)
{
  double delay = time(NULL) - timestamp;
  return delay < 0 || delay > DEFAULT_MSG_DELAY;
}

pending_msg_t* get_msg_from_checksum(const char *checksum)
{
  for (int i = 0; i <= msginterface_queue->max_index; i++)
  {
    pending_msg_t *pending_msg = queue_get(msginterface_queue, i);
    if (strcmp(pending_msg->checksum, checksum) == 0)
    {
      return pending_msg;
    }
  }
  return NULL;
}

void free_msg(pending_msg_t *pending_msg)
{
  pending_msg->id = -1;
  pending_msg->checksum = NULL;
  pending_msg->size = 0;
  pending_msg->timestamp = 0;
  free(pending_msg);
}

void free_msg_by_id(int id)
{
  pending_msg_t *pending_msg = get_msg_from_id(id);
  if (!pending_msg)
  {
    return;
  }
  free_msg(pending_msg);
}

task_result_t poll_msginterface(task_t *task, va_list args)
{
  if (!queue_get_empty(msginterface_queue))
  {
    pending_msg_t *pending_msg = queue_pop_left(msginterface_queue);
    if (!pending_msg)
    {
      return TASK_RESULT_CONT;
    }
    if (!get_msg_has_expired(pending_msg->timestamp))
    {
      queue_push_right(msginterface_queue, pending_msg);
    }
    else
    {
      free_msg(pending_msg);
    }
  }
  return TASK_RESULT_CONT;
}
