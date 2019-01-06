/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, December 28th, 2018
 */

#include <stdio.h>
#include <assert.h>

#include "log.h"
#include "task.h"

task_result_t standard_task_func(task_t *task)
{
  log_info("Task with id <%d>, running...", task->id);
  return TASK_RESULT_DONE;
}

task_result_t delayed_task_func(task_t *task)
{
  log_info("Task with id <%d>, running delayed <%f seconds>...", task->id, task->delay);
  return TASK_RESULT_DONE;
}

int main(int argc, char **argv)
{
  taskmgr_init();

  task_t *task0 = add_task(standard_task_func, 0);
  task_t *task1 = add_task(standard_task_func, 0);

  task_t *delayed_task0 = add_task(delayed_task_func, 5);
  task_t *delayed_task1 = add_task(delayed_task_func, 5);

  taskmgr_run();
  taskmgr_shutdown();
  return 0;
}
