# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
# - Find LibEvent (a cross event library)
# This module defines
# LIB_EVENT_INCLUDE_DIR, where to find LibEvent headers
# LIB_EVENT_LIBRARY, LibEvent libraries
# LibEvent_FOUND, If false, do not try to use libevent

set(LibEvent_EXTRA_PREFIXES /usr/local /opt/local "$ENV{HOME}")
foreach(prefix ${LibEvent_EXTRA_PREFIXES})
  list(APPEND LibEvent_INCLUDE_PATHS "${prefix}/include")
  list(APPEND LIB_EVENT_LIBRARY_PATHS "${prefix}/lib")
endforeach()

find_path(LIB_EVENT_INCLUDE_DIR event.h PATHS ${LibEvent_INCLUDE_PATHS})
find_library(LIB_EVENT_LIBRARY NAMES event PATHS ${LIB_EVENT_LIBRARY_PATHS})
find_library(LIBEVENT_PTHREAD_LIB NAMES event_pthreads PATHS ${LIB_EVENT_LIBRARY_PATHS})

if (LIB_EVENT_LIBRARY AND LIB_EVENT_INCLUDE_DIR AND LIBEVENT_PTHREAD_LIB)
  set(LibEvent_FOUND TRUE)
  set(LIB_EVENT_LIBRARY ${LIB_EVENT_LIBRARY} ${LIBEVENT_PTHREAD_LIB})
else ()
  set(LibEvent_FOUND FALSE)
endif ()

if (LibEvent_FOUND)
  if (NOT LibEvent_FIND_QUIETLY)
    message(STATUS "Found libevent: ${LIB_EVENT_LIBRARY}")
  endif ()
else ()
  if (LibEvent_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find libevent and libevent_pthread.")
  endif ()
  message(STATUS "libevent and libevent_pthread NOT found.")
endif ()

mark_as_advanced(
    LIB_EVENT_LIBRARY
    LIB_EVENT_INCLUDE_DIR
  )

