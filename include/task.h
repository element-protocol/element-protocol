/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <pthread.h>

#include "queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum TaskResult
{
  TASK_RESULT_DONE = 0,
  TASK_RESULT_CONT,
  TASK_RESULT_WAIT
} task_result_t;

typedef task_result_t (*callable_func_t)();

typedef struct Task
{
  int id;
  callable_func_t func;
  va_list *args;
  bool delayable;
  double delay;
  time_t timestamp;
  pthread_mutex_t mutex;
} task_t;

typedef struct TaskScheduler
{
  int id;
  pthread_t thread;
  pthread_attr_t thread_attr;
} task_scheduler_t;

static int taskmgr_next_task_id = -1;
static int taskmgr_next_scheduler_id = -1;

static queue_t *taskmgr_task_queue;
static queue_t *taskmgr_scheduler_queue;

static bool taskmgr_terminated = false;

bool taskmgr_init(void);
void taskmgr_run(void);
void* taskmgr_scheduler_run();
bool taskmgr_shutdown(void);

bool has_task(task_t *task);
bool has_task_by_id(int id);

task_t* add_task(callable_func_t func, double delay, ...);

task_t* get_task_by_id(int id);

void remove_task(task_t *task);
void remove_task_by_id(int id);

void free_task(task_t *task);
void free_task_by_id(int id);

bool has_scheduler(task_scheduler_t *task_scheduler);
bool has_scheduler_by_id(int id);

task_scheduler_t* add_scheduler(void);

task_scheduler_t* get_scheduler_by_id(int id);

void remove_scheduler(task_scheduler_t *task_scheduler);
void remove_scheduler_by_id(int id);

void free_scheduler(task_scheduler_t *task_scheduler);
void free_scheduler_by_id(int id);

#ifdef __cplusplus
}
#endif
