/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "queue.h"

queue_t* queue_init(void)
{
  queue_t *queue = malloc(sizeof(queue_t));
  queue->num_objects = 0;
  queue->max_index = -1;

  pthread_mutex_init(&queue->mutex, NULL);
  return queue;
}

void queue_free(queue_t *queue)
{
  for (int i = 0; i <= queue->max_index; i++)
  {
    queue_remove(queue, i);
  }
  queue->num_objects = 0;
  queue->max_index = -1;

  pthread_mutex_destroy(&queue->mutex);
  free(queue);
}

int queue_get_size(queue_t *queue)
{
  return queue->num_objects;
}

bool queue_get_empty(queue_t *queue)
{
  return queue->num_objects == 0;
}

int queue_get_max_index(queue_t *queue)
{
  return queue->max_index;
}

int queue_get_index(queue_t *queue, void *queue_object)
{
  int index = -1;
  for (int i = 0; i <= queue->max_index; i++)
  {
    // find this objects index in the array
    if (queue->queue_objects[i] == queue_object)
    {
      index = i;
      break;
    }
  }
  return index;
}

void* queue_get(queue_t *queue, int index)
{
  if (index < 0)
  {
    return NULL;
  }
  return queue->queue_objects[index];
}

void queue_push(queue_t *queue, int index, void *queue_object)
{
  if (index < 0)
  {
    return;
  }
  queue->queue_objects[index] = queue_object;
  queue->num_objects++;
  if (index > queue->max_index)
  {
    queue->max_index = index;
  }
}

void queue_push_left(queue_t *queue, void *queue_object)
{
  pthread_mutex_lock(&queue->mutex);
  for (int i = queue->max_index + 1; i >= 0; i--)
  {
    // shift all object over right by one index
    queue->queue_objects[i] = queue->queue_objects[i - 1];
  }
  queue_push(queue, 0, queue_object);
  pthread_mutex_unlock(&queue->mutex);
}

void queue_push_right(queue_t *queue, void *queue_object)
{
  pthread_mutex_lock(&queue->mutex);
  queue_push(queue, queue->max_index + 1, queue_object);
  pthread_mutex_unlock(&queue->mutex);
}

void queue_remove(queue_t *queue, int index)
{
  if (index < 0)
  {
    return;
  }
  if (index >= queue->max_index)
  {
    queue->max_index = index - 1;
  }
  queue->queue_objects[index] = NULL;
  queue->num_objects--;
}

void queue_remove_object(queue_t *queue, void *queue_object)
{
  if (!queue_object)
  {
    return;
  }
  pthread_mutex_lock(&queue->mutex);
  int index = queue_get_index(queue, queue_object);
  pthread_mutex_unlock(&queue->mutex);
  if (index == -1)
  {
    return;
  }
  pthread_mutex_lock(&queue->mutex);
  queue_remove(queue, index);
  pthread_mutex_unlock(&queue->mutex);
}

void* queue_pop(queue_t *queue, int index)
{
  void *queue_object = queue_get(queue, index);
  queue_remove(queue, index);
  return queue_object;
}

void* queue_pop_left(queue_t *queue)
{
  pthread_mutex_lock(&queue->mutex);
  void *queue_object = queue_pop(queue, 0);
  for (int i = 1; i <= queue->max_index; i++)
  {
    // shift all object over left by one index
    queue->queue_objects[i - 1] = queue->queue_objects[i];
  }

  // when shifting objects over left, the maximum index in the queue
  // will remain unused in memory until cleared
  queue->queue_objects[queue->max_index] = NULL;
  queue->max_index--;
  pthread_mutex_unlock(&queue->mutex);
  return queue_object;
}

void* queue_pop_right(queue_t *queue)
{
  pthread_mutex_lock(&queue->mutex);
  void *queue_object = queue_pop(queue, queue->max_index);
  pthread_mutex_unlock(&queue->mutex);
  return queue_object;
}
