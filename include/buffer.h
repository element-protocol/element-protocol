/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Buffer
{
  unsigned char *data;
  int size;
  int offset;
} buffer_t;

buffer_t* buffer_init_data(int offset, const unsigned char *data, int size);
buffer_t* buffer_init_size(int offset, int size);
buffer_t* buffer_init_offset(int offset);
buffer_t* buffer_init(void);
void buffer_copy(buffer_t *buffer, buffer_t *other_buffer);
void buffer_free(buffer_t *buffer);

void buffer_realloc(buffer_t *buffer, int size);
void buffer_write(buffer_t *buffer, const unsigned char *data, int size);
char* buffer_read(buffer_t *buffer, int size);
int buffer_get_size(buffer_t *buffer);
int buffer_get_remaining_size(buffer_t *buffer);
const unsigned char* buffer_get_data(buffer_t *buffer);
const unsigned char* buffer_get_remaining_data(buffer_t *buffer);

void buffer_write_uint8(buffer_t *buffer, uint8_t value);
uint8_t buffer_read_uint8(buffer_t *buffer);
void buffer_write_int8(buffer_t *buffer, int8_t value);
int8_t buffer_read_int8(buffer_t *buffer);

void buffer_write_uint16(buffer_t *buffer, uint16_t value);
uint16_t buffer_read_uint16(buffer_t *buffer);
void buffer_write_int16(buffer_t *buffer, int16_t value);
int16_t buffer_read_int16(buffer_t *buffer);

void buffer_write_uint32(buffer_t *buffer, uint32_t value);
uint32_t buffer_read_uint32(buffer_t *buffer);
void buffer_write_int32(buffer_t *buffer, int32_t value);
int32_t buffer_read_int32(buffer_t *buffer);

void buffer_write_uint64(buffer_t *buffer, uint64_t value);
uint64_t buffer_read_uint64(buffer_t *buffer);
void buffer_write_int64(buffer_t *buffer, int64_t value);
int64_t buffer_read_int64(buffer_t *buffer);

void buffer_write_string(buffer_t *buffer, const char *string, int size);
char* buffer_read_string(buffer_t *buffer);

#ifdef __cplusplus
}
#endif
