/*
 * Copyright (C) Caleb Marshall - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Caleb Marshall <anythingtechpro@gmail.com>, November 14th, 2018
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

int get_num_logical_cores(void);

bool string_equals(const char *string, const char *equals);
bool string_startswith(const char *string, const char *prefix);
bool string_endswith(const char *string, const char *ext);
int string_count(const char *string, const char *countstr, bool countbreak);

#ifdef __cplusplus
}
#endif
