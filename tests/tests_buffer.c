/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, December 28th, 2018
 */

#include <stdio.h>
#include <assert.h>

#include "buffer.h"
#include "util.h"

int main(int argc, char **argv)
{
  // pack
  buffer_t *buffer = buffer_init();

  const char *msg = "Hello World!";
  buffer_write_string(buffer, msg, strlen(msg));

  const char *msg1 = "The quick brown fox jumps over the lazy dog.";
  buffer_write_string(buffer, msg1, strlen(msg1));

  const char *msg2 = "THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK 1234567890";
  buffer_write_string(buffer, msg2, strlen(msg2));

  buffer_t *buffer2 = buffer_init_data(0, buffer_get_data(buffer), buffer_get_size(buffer));

  buffer_free(buffer);

  // unpack
  char *unpacked_msg = buffer_read_string(buffer2);
  assert(string_equals(unpacked_msg, msg)); // check result
  free(unpacked_msg);

  char *unpacked_msg1 = buffer_read_string(buffer2);
  assert(string_equals(unpacked_msg1, msg1)); // check result
  free(unpacked_msg1);

  char *unpacked_msg2 = buffer_read_string(buffer2);
  assert(string_equals(unpacked_msg2, msg2)); // check result
  free(unpacked_msg2);

  assert(buffer_get_remaining_size(buffer2) == 0); // check remaining size

  buffer_free(buffer2);
  return 0;
}
