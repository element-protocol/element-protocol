# Copyright (C) Caleb Marshall - All Rights Reserved
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
# Written by Caleb Marshall <anythingtechpro@gmail.com>, November 2nd, 2018

find_path(SODIUM_INCLUDE_DIR
    sodium.h
  PATH_SUFFIXES
    include
  PATHS
    ${SODIUMDIR}
    ${SODIUMDIR}/src/libsodium
    $ENV{SODIUMDIR}
    $ENV{SODIUMDIR}/src/libsodium
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/
    /usr/
    /sw          # Fink
    /opt/local/  # DarwinPorts
    /opt/csw/    # Blastwave
    /opt/
)

set(SODIUM_FOUND TRUE)
set(FIND_SODIUM_LIB_PATHS
  ${SODIUMDIR}
  $ENV{SODIUMDIR}
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

find_library(SODIUM_LIBRARY_DEBUG
  NAMES
    sodium
    libsodium
  PATH_SUFFIXES
    lib64
    lib
  PATHS
    ${FIND_SODIUM_LIB_PATHS}
    ${SODIUMDIR}/bin/Win32/Debug/v120/dynamic
    $ENV{SODIUMDIR}/bin/Win32/Debug/v120/dynamic
)

find_library(SODIUM_LIBRARY_RELEASE
  NAMES
    sodium
    libsodium
  PATH_SUFFIXES
    lib64
    lib
  PATHS
    ${FIND_SODIUM_LIB_PATHS}
    ${SODIUMDIR}/bin/Win32/Release/v120/dynamic
    $ENV{SODIUMDIR}/bin/Win32/Release/v120/dynamic
)

if (SODIUM_LIBRARY_DEBUG OR SODIUM_LIBRARY_RELEASE)
  if (SODIUM_LIBRARY_DEBUG AND SODIUM_LIBRARY_RELEASE)
    set(SODIUM_LIBRARY debug ${SODIUM_LIBRARY_DEBUG} optimized ${SODIUM_LIBRARY_RELEASE})
  endif()

  if (SODIUM_LIBRARY_DEBUG AND NOT SODIUM_LIBRARY_RELEASE)
    set(SODIUM_LIBRARY_RELEASE ${SODIUM_LIBRARY_DEBUG})
    set(SODIUM_LIBRARY ${SODIUM_LIBRARY_DEBUG})
  endif()
  if (SODIUM_LIBRARY_RELEASE AND NOT SODIUM_LIBRARY_DEBUG)
    set(SODIUM_LIBRARY_DEBUG ${SODIUM_LIBRARY_RELEASE})
    set(SODIUM_LIBRARY ${SODIUM_LIBRARY_RELEASE})
  endif()
else()
  set(SODIUM_FOUND FALSE)
  set(SODIUM_LIBRARY "")
  set(FIND_SODIUM_MISSING "${FIND_SODIUM_MISSING} SODIUM_LIBRARY")
endif()

mark_as_advanced(
  SODIUM_LIBRARY
  SODIUM_LIBRARY_RELEASE
  SODIUM_LIBRARY_DEBUG
)

set(SODIUM_LIBRARIES ${SODIUM_LIBRARIES} "${SODIUM_LIBRARY}")

if (NOT SODIUM_FOUND)
  set(FIND_SODIUM_ERROR "Could NOT find Sodium (missing: ${FIND_SODIUM_MISSING})")
  if(SODIUM_FIND_REQUIRED)
    message(FATAL_ERROR ${FIND_SODIUM_ERROR})
  elseif(NOT SODIUM_FIND_QUIETLY)
    message("${FIND_SODIUM_ERROR}")
  endif()
endif()

if(SODIUM_FOUND)
  message("Found Sodium: ${SODIUM_INCLUDE_DIR}")
endif()
