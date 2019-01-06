/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 14th, 2018
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN32
 #include <sysinfoapi.h>
#endif

#include "util.h"

int get_num_logical_cores(void)
{
#ifdef _WIN32
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

bool string_equals(const char *string, const char *equals)
{
  return strcmp(string, equals) == 0;
}

bool string_startswith(const char *string, const char *prefix)
{
  return strncmp(prefix, string, strlen(prefix)) == 0;
}

bool string_endswith(const char *string, const char *ext)
{
  int ext_length = strlen(ext);
  return strncmp(ext, &string[strlen(string) - ext_length], ext_length) == 0;
}

int string_count(const char *string, const char *countstr, bool countbreak)
{
  int count = 0;
  for (int i = 0; i < strlen(string) - 1; i++)
  {
    if (string_startswith(&string[i], countstr))
    {
      count++;
    }
    else
    {
      if (countbreak)
      {
        break;
      }
    }
  }
  return count;
}
