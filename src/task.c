/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdbool.h>
#include <stdarg.h>
#include <sched.h>
#include <pthread.h>

#include "log.h"
#include "util.h"

#include "task.h"

bool taskmgr_init(void)
{
  taskmgr_task_queue = queue_init();
  taskmgr_scheduler_queue = queue_init();

  log_info("Initialized taskmgr.");
  return true;
}

void taskmgr_run(void)
{
  while (!taskmgr_terminated)
  {
    if (!queue_get_empty(taskmgr_task_queue))
    {
      task_t *task = queue_pop_left(taskmgr_task_queue);
      if (!task)
      {
        continue;
      }
      if (task->delayable)
      {
        if (time(NULL) - task->timestamp < task->delay)
        {
          // put the task back into the queue since it's not
          // time yet to call it...
          queue_push_right(taskmgr_task_queue, task);
          continue;
        }
      }
      pthread_mutex_lock(&task->mutex);
      va_list args;
      va_copy(args, *task->args);
      task_result_t result = task->func(task, args);
      va_end(args);
      pthread_mutex_unlock(&task->mutex);

      // process the task result and determine what the
      // task should do next...
      switch (result)
      {
        case TASK_RESULT_CONT:
          task->delayable = false;
          queue_push_right(taskmgr_task_queue, task);
          break;
        case TASK_RESULT_WAIT:
          task->delayable = true;
          task->timestamp = time(NULL);
          queue_push_right(taskmgr_task_queue, task);
          break;
        case TASK_RESULT_DONE:
        default:
          free_task(task);
          break;
      }
    }
    sched_yield();
  }
}

void* taskmgr_scheduler_run()
{
  taskmgr_run();
  return NULL;
}

bool taskmgr_shutdown(void)
{
  taskmgr_terminated = true;

  queue_free(taskmgr_task_queue);
  queue_free(taskmgr_scheduler_queue);

  log_info("Shutdown taskmgr.");
  return true;
}

bool has_task(task_t *task)
{
  return queue_get_index(taskmgr_task_queue, task) != -1;
}

bool has_task_by_id(int id)
{
  return get_task_by_id(id) != NULL;
}

task_t* add_task(callable_func_t func, double delay, ...)
{
  va_list args;
  va_start(args, delay);

  taskmgr_next_task_id++;

  task_t* task = malloc(sizeof(task_t));
  task->id = taskmgr_next_task_id;
  task->func = func;
  task->args = &args;
  task->delayable = true;
  task->delay = delay;
  task->timestamp = time(NULL);

  pthread_mutex_init(&task->mutex, NULL);
  queue_push_right(taskmgr_task_queue, task);
  return task;
}

task_t* get_task_by_id(int id)
{
  for (int i = 0; i <= taskmgr_task_queue->max_index; i++)
  {
    task_t *task = queue_get(taskmgr_task_queue, i);
    if (task->id == id)
    {
      return task;
    }
  }
  return NULL;
}

void remove_task(task_t *task)
{
  queue_remove_object(taskmgr_task_queue, task);
  free_task(task);
}

void remove_task_by_id(int id)
{
  task_t *task = get_task_by_id(id);
  if (!task)
  {
    return;
  }
  remove_task(task);
}

void free_task(task_t *task)
{
  va_end(*task->args);

  task->id = -1;
  task->delayable = false;
  task->delay = 0;
  task->timestamp = 0;

  pthread_mutex_destroy(&task->mutex);
  free(task);
}

void free_task_by_id(int id)
{
  task_t *task = get_task_by_id(id);
  if (!task)
  {
    return;
  }
  free_task(task);
}

bool has_scheduler(task_scheduler_t *task_scheduler)
{
  return queue_get_index(taskmgr_scheduler_queue, task_scheduler) != -1;
}

bool has_scheduler_by_id(int id)
{
  return get_scheduler_by_id(id) != NULL;
}

task_scheduler_t* add_scheduler(void)
{
  taskmgr_next_scheduler_id++;

  task_scheduler_t* task_scheduler = malloc(sizeof(task_scheduler_t));
  task_scheduler->id = taskmgr_next_scheduler_id;

  if (pthread_attr_init(&task_scheduler->thread_attr) != 0)
  {
    log_error("Failed to initialize thread attribute!");
    return NULL;
  }
  if (pthread_create(&task_scheduler->thread, &task_scheduler->thread_attr,
    taskmgr_scheduler_run, NULL) != 0)
  {
    log_error("Failed to initialize thread!");
    return NULL;
  }
  queue_push_right(taskmgr_scheduler_queue, task_scheduler);
  return task_scheduler;
}

task_scheduler_t* get_scheduler_by_id(int id)
{
  for (int i = 0; i <= taskmgr_scheduler_queue->max_index; i++)
  {
    task_scheduler_t *task_scheduler = queue_get(taskmgr_scheduler_queue, i);
    if (task_scheduler->id == id)
    {
      return task_scheduler;
    }
  }
  return NULL;
}

void remove_scheduler(task_scheduler_t *task_scheduler)
{
  queue_remove_object(taskmgr_scheduler_queue, task_scheduler);
  free_scheduler(task_scheduler);
}

void remove_scheduler_by_id(int id)
{
  task_scheduler_t *task_scheduler = get_scheduler_by_id(id);
  if (!task_scheduler)
  {
    return;
  }
  remove_scheduler(task_scheduler);
}

void free_scheduler(task_scheduler_t *task_scheduler)
{
  task_scheduler->id = -1;
  pthread_attr_destroy(&task_scheduler->thread_attr);
  free(task_scheduler);
}

void free_scheduler_by_id(int id)
{
  task_scheduler_t *task_scheduler = get_scheduler_by_id(id);
  if (!task_scheduler)
  {
    return;
  }
  free_scheduler(task_scheduler);
}
