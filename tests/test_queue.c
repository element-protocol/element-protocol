/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, December 28th, 2018
 */

#include <stdio.h>
#include <assert.h>

#include "queue.h"

typedef struct TestQueueObject
{
  int a;
  int b;
  int c;
} test_queue_object_t;

int main(int argc, char **argv)
{
  queue_t *queue = queue_init();

  assert(queue_get_size(queue) == 0);

  int *a = 0;
  queue_push_left(queue, a);
  assert(queue_pop_right(queue) == a);
  assert(queue_get_size(queue) == 0);

  int *b = 0;
  queue_push_right(queue, b);
  assert(queue_pop_left(queue) == b);
  assert(queue_get_size(queue) == 0);

  test_queue_object_t *test_queue_object = malloc(sizeof(test_queue_object_t));
  test_queue_object->a = 0;
  test_queue_object->b = 1;
  test_queue_object->c = 2;

  queue_push_left(queue, test_queue_object);
  assert(queue_pop_left(queue) == test_queue_object);
  assert(queue_get_size(queue) == 0);

  queue_push_right(queue, test_queue_object);
  assert(queue_pop_right(queue) == test_queue_object);
  assert(queue_get_size(queue) == 0);
  assert(queue_get_max_index(queue) == -1);

  free(test_queue_object);
  queue_free(queue);
  return 0;
}
