# Copyright (C) Caleb Marshall - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
# Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

cmake_minimum_required(VERSION 3.8)
project(element)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set(MINIUPNP_STATIC ON)

find_package(Threads QUIET)
if(NOT Threads_FOUND)
  message(FATAL_ERROR "Failed to find pthreads dependency!")
endif()

find_package(Sodium QUIET)
if(NOT SODIUM_FOUND)
  message(FATAL_ERROR "Failed to find libsodium dependency!")
endif()

include_directories(${SODIUM_INCLUDE_DIR})
include_directories(include)

add_subdirectory(external/miniupnp/miniupnpc)
add_subdirectory(src)
add_subdirectory(tests)
