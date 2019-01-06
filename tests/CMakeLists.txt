# Copyright (C) Caleb Marshall - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
# Written by Caleb Marshall <anythingtechpro@gmail.com>, December 27th, 2018

set(TESTBUFFER_SOURCES
  ${PROJECT_SOURCE_DIR}/src/buffer.c
  ${PROJECT_SOURCE_DIR}/src/util.c
  tests_buffer.c
)

set(TESTBUFFER_HEADERS
  ${PROJECT_SOURCE_DIR}/include/buffer.h
  ${PROJECT_SOURCE_DIR}/include/util.h
)

add_executable(
  tests_buffer
  ${TESTBUFFER_SOURCES}
  ${TESTBUFFER_HEADERS}
)

set(TESTTASK_SOURCES
  ${PROJECT_SOURCE_DIR}/src/log.c
  ${PROJECT_SOURCE_DIR}/src/queue.c
  ${PROJECT_SOURCE_DIR}/src/task.c
  tests_tasks.c
)

set(TESTTASK_HEADERS
  ${PROJECT_SOURCE_DIR}/include/log.h
  ${PROJECT_SOURCE_DIR}/include/queue.h
  ${PROJECT_SOURCE_DIR}/include/task.h
)

add_executable(
  tests_tasks
  ${TESTTASK_SOURCES}
  ${TESTTASK_HEADERS}
)

set(TESTQUEUE_SOURCES
  ${PROJECT_SOURCE_DIR}/src/queue.c
  test_queue.c
)

set(TESTQUEUE_HEADERS
  ${PROJECT_SOURCE_DIR}/include/queue.h
)

add_executable(
  test_queue
  ${TESTQUEUE_SOURCES}
  ${TESTQUEUE_HEADERS}
)
