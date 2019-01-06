/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 20th, 2018
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "queue.h"
#include "task.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DEFAULT_MSG_DELAY 60.0

typedef struct PendingMsg
{
  int id;
  const char *checksum;
  int size;
  time_t timestamp;
} pending_msg_t;

static int msginterface_next_id = -1;
static queue_t *msginterface_queue;
static task_t *msginterface_poll_task;

bool msginterface_init(void);
bool msginterface_shutdown(void);

bool has_msg(pending_msg_t *pending_msg);
bool has_msg_by_id(int id);
bool has_msg_by_checksum(const char *checksum);

pending_msg_t* add_msg(const char* checksum, int size, time_t timestamp);

void remove_msg(pending_msg_t *pending_msg);
void remove_msg_by_id(int id);

pending_msg_t* get_msg_from_id(int id);
pending_msg_t* get_msg_from_checksum(const char *checksum);
bool get_msg_has_expired(time_t timestamp);

void free_msg(pending_msg_t *pending_msg);
void free_msg_by_id(int id);

task_result_t poll_msginterface(task_t *task, va_list args);

#ifdef __cplusplus
}
#endif
