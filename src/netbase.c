/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 25th, 2018
 */

#include <stdbool.h>
#include <arpa/inet.h>

#include "util.h"

#include "netbase.h"

bool netbase_get_is_valid_address(const char* address)
{
  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, address, &(sa.sin_addr));
  return result != 0;
}

bool netbase_get_is_local_address(const char *address)
{
  return string_equals(address, DEFAULT_ADDRESS) || string_equals(address, DEFAULT_LOCAL_ADDRESS);
}
