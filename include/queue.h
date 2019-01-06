/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_QUEUE_SIZE 100000

typedef struct Queue
{
  int num_objects;
  int max_index;
  void *queue_objects[MAX_QUEUE_SIZE];
  pthread_mutex_t mutex;
} queue_t;

queue_t* queue_init(void);
void queue_free(queue_t *queue);

void queue_push(queue_t *queue, int index, void *queue_object);
void queue_push_left(queue_t *queue, void *queue_object);
void queue_push_right(queue_t *queue, void *queue_object);

int queue_get_size(queue_t *queue);
bool queue_get_empty(queue_t *queue);
int queue_get_max_index(queue_t *queue);
int queue_get_index(queue_t *queue, void *queue_object);
void* queue_get(queue_t *queue, int index);

void queue_remove(queue_t *queue, int index);
void queue_remove_object(queue_t *queue, void *queue_object);

void* queue_pop(queue_t *queue, int index);
void* queue_pop_left(queue_t *queue);
void* queue_pop_right(queue_t *queue);

#ifdef __cplusplus
}
#endif
