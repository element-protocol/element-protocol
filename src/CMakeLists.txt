# Copyright (C) Caleb Marshall - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
# Written by Caleb Marshall <anythingtechpro@gmail.com>, December 27th, 2018

set(ELEMENT_SOURCES
  aes.c
  base64.c
  buffer.c
  crypto.c
  dyad.c
  keypairinterface.c
  log.c
  main.c
  msginterface.c
  msgprotocol.c
  net.c
  netbase.c
  netinterface.c
  p2p.c
  protocol.c
  queue.c
  task.c
  util.c
)

set(ELEMENT_HEADERS
  ${PROJECT_SOURCE_DIR}/include/aes.h
  ${PROJECT_SOURCE_DIR}/include/base64.h
  ${PROJECT_SOURCE_DIR}/include/buffer.h
  ${PROJECT_SOURCE_DIR}/include/crypto.h
  ${PROJECT_SOURCE_DIR}/include/dyad.h
  ${PROJECT_SOURCE_DIR}/include/keypairinterface.h
  ${PROJECT_SOURCE_DIR}/include/log.h
  ${PROJECT_SOURCE_DIR}/include/msginterface.h
  ${PROJECT_SOURCE_DIR}/include/msgprotocol.h
  ${PROJECT_SOURCE_DIR}/include/net.h
  ${PROJECT_SOURCE_DIR}/include/netbase.h
  ${PROJECT_SOURCE_DIR}/include/netinterface.h
  ${PROJECT_SOURCE_DIR}/include/p2p.h
  ${PROJECT_SOURCE_DIR}/include/protocol.h
  ${PROJECT_SOURCE_DIR}/include/protocolbase.h
  ${PROJECT_SOURCE_DIR}/include/queue.h
  ${PROJECT_SOURCE_DIR}/include/task.h
  ${PROJECT_SOURCE_DIR}/include/util.h
  ${PROJECT_SOURCE_DIR}/include/version.h
)

add_executable(
  ${PROJECT_NAME}
  ${ELEMENT_SOURCES}
  ${ELEMENT_HEADERS}
)

target_link_libraries(
  ${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)

if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
  target_link_libraries(
    ${PROJECT_NAME}
    ${SODIUM_LIBRARY_DEBUG}
  )
else()
  target_link_libraries(
    ${PROJECT_NAME}
    ${SODIUM_LIBRARY_RELEASE}
  )
endif(CMAKE_BUILD_TYPE EQUAL "DEBUG")

if(MINIUPNP_STATIC)
  target_link_libraries(
    ${PROJECT_NAME}
    libminiupnpc-static
  )
else()
  target_link_libraries(
    ${PROJECT_NAME}
    libminiupnpc-shared
  )
endif()
