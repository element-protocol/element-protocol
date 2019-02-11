/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

#include "buffer.h"

buffer_t* buffer_init_data(int offset, const unsigned char *data, int size)
{
  buffer_t *buffer = malloc(sizeof(buffer_t));
  buffer->data = NULL;

  // only copy the data over if the array actually
  // contains any data...
  if (size > 0)
  {
    buffer->data = malloc(size);
    memcpy(buffer->data, data, size);
  }
  buffer->size = size;
  buffer->offset = offset;
  return buffer;
}

buffer_t* buffer_init_size(int offset, int size)
{
  const unsigned char *data = NULL;
  return buffer_init_data(offset, data, size);
}

buffer_t* buffer_init_offset(int offset)
{
  return buffer_init_size(offset, 0);
}

buffer_t* buffer_init(void)
{
  return buffer_init_offset(0);
}

void buffer_copy(buffer_t *buffer, buffer_t *other_buffer)
{
  buffer_realloc(buffer, other_buffer->size);
  memcpy(buffer->data, other_buffer->data, other_buffer->size);
  buffer->size = other_buffer->size;
  buffer->offset = other_buffer->offset;
}

void buffer_free(buffer_t *buffer)
{
  if (buffer->data != NULL)
  {
    free(buffer->data);
    buffer->data = NULL;
  }
  buffer->size = 0;
  buffer->offset = 0;
  free(buffer);
}

void buffer_realloc(buffer_t *buffer, int size)
{
  // resize the array if there isn't enough memory
  // pre-allocated...
  if (buffer_get_remaining_size(buffer) < size)
  {
    unsigned char *data = malloc(buffer->size + size);
    memcpy(data, buffer->data, buffer->size);
    buffer->data = data;
    buffer->size += size;
  }
}

void buffer_write(buffer_t *buffer, const unsigned char *data, int size)
{
  buffer_realloc(buffer, size);
  memcpy(buffer->data + buffer->offset, data, size);
  buffer->offset += size;
}

char* buffer_read(buffer_t *buffer, int size)
{
  unsigned char *data = malloc(size);
  memcpy(data, buffer->data + buffer->offset, size);
  buffer->offset += size;
  return (char*)data;
}

int buffer_get_size(buffer_t *buffer)
{
  return buffer->size;
}

int buffer_get_remaining_size(buffer_t *buffer)
{
  return buffer_get_size(buffer) - buffer->offset;
}

const unsigned char* buffer_get_data(buffer_t *buffer)
{
  return buffer->data;
}

const unsigned char* buffer_get_remaining_data(buffer_t *buffer)
{
  return buffer->data + buffer->offset;
}

void buffer_write_uint8(buffer_t *buffer, uint8_t value)
{
  int size = sizeof(uint8_t);
  buffer_realloc(buffer, size);
  *(uint8_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

uint8_t buffer_read_uint8(buffer_t *buffer)
{
  uint8_t value = *(uint8_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(uint8_t);
  return value;
}

void buffer_write_int8(buffer_t *buffer, int8_t value)
{
  int size = sizeof(int8_t);
  buffer_realloc(buffer, size);
  *(int8_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

int8_t buffer_read_int8(buffer_t *buffer)
{
  int8_t value = *(int8_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(int8_t);
  return value;
}

void buffer_write_uint16(buffer_t *buffer, uint16_t value)
{
  int size = sizeof(uint16_t);
  buffer_realloc(buffer, size);
  *(uint16_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

uint16_t buffer_read_uint16(buffer_t *buffer)
{
  uint16_t value = *(uint16_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(uint16_t);
  return value;
}

void buffer_write_int16(buffer_t *buffer, int16_t value)
{
  int size = sizeof(int16_t);
  buffer_realloc(buffer, size);
  *(int16_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

int16_t buffer_read_int16(buffer_t *buffer)
{
  int16_t value = *(int16_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(int16_t);
  return value;
}

void buffer_write_uint32(buffer_t *buffer, uint32_t value)
{
  int size = sizeof(uint32_t);
  buffer_realloc(buffer, size);
  *(uint32_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

uint32_t buffer_read_uint32(buffer_t *buffer)
{
  uint32_t value = *(uint32_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(uint32_t);
  return value;
}

void buffer_write_int32(buffer_t *buffer, int32_t value)
{
  int size = sizeof(int32_t);
  buffer_realloc(buffer, size);
  *(int32_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

int32_t buffer_read_int32(buffer_t *buffer)
{
  int32_t value = *(int32_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(int32_t);
  return value;
}

void buffer_write_uint64(buffer_t *buffer, uint64_t value)
{
  int size = sizeof(uint64_t);
  buffer_realloc(buffer, size);
  *(uint64_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

uint64_t buffer_read_uint64(buffer_t *buffer)
{
  uint64_t value = *(uint64_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(uint64_t);
  return value;
}

void buffer_write_int64(buffer_t *buffer, int64_t value)
{
  int size = sizeof(int64_t);
  buffer_realloc(buffer, size);
  *(int64_t*)(buffer->data + buffer->offset) = value;
  buffer->offset += size;
}

int64_t buffer_read_int64(buffer_t *buffer)
{
  int64_t value = *(int64_t*)(buffer->data + buffer->offset);
  buffer->offset += sizeof(int64_t);
  return value;
}

void buffer_write_string(buffer_t *buffer, const char *string, int size)
{
  int actual_size = sizeof(unsigned char*) + size;
  buffer_write_uint16(buffer, actual_size);
  buffer_write(buffer, (const unsigned char*)string, actual_size);
}

char* buffer_read_string(buffer_t *buffer)
{
  return (char*)buffer_read(buffer, buffer_read_uint16(buffer));
}
