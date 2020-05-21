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
# copied in most part from protobuf cmake
# there are similar protobuf gen changes online, all of which do
# a similar job of customizing their generation.
function(generate_protobuf_src SRCS HDRS HDR_DIR PROTO_PATH)
  if(NOT ARGN)
    message(SEND_ERROR "Error: generate_protobuf_src() called without any proto files")
    return()
  endif()
  	message(STATUS "Generating proto files using proto path: ${PROTO_PATH}")
  set(_protobuf_include_path -I .)
  if(DEFINED PROTOBUF_INCLUDE_DIRS)
    foreach(DIR ${PROTOBUF_INCLUDE_DIRS})
      file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${DIR})
      list(FIND _protobuf_include_path ${REL_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
        list(APPEND _protobuf_include_path -I ${REL_PATH})
      endif()
    endforeach()
  endif()
  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
  	message("Generating ${FIL} with ${PROTOBUF_PROTOC_EXECUTABLE}")
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    ## get the directory where our protobufs are stored
    file(RELATIVE_PATH REL_FIL ${CMAKE_SOURCE_DIR} ${ABS_FIL})
    file(RELATIVE_PATH REL_FILE_TO_BASE ${PROTO_PATH} ${ABS_FIL})
    get_filename_component(REL_DIR ${REL_FIL} DIRECTORY)
    get_filename_component(REL_FILE_TO_BASE_DIR ${REL_FILE_TO_BASE} DIRECTORY)
	set(REL_FILE_TO_BASE "${REL_FILE_TO_BASE_DIR}/${FIL_WE}")    
	set(${HDR_DIR} "${CMAKE_BINARY_DIR_GEN}/" PARENT_SCOPE)
    list(APPEND ${SRCS} "${CMAKE_BINARY_DIR_GEN}/${REL_FILE_TO_BASE}.pb.cc")
    list(APPEND ${HDRS} "${CMAKE_BINARY_DIR_GEN}/${REL_FILE_TO_BASE}.pb.h")
    add_custom_command(
      OUTPUT "${CMAKE_BINARY_DIR_GEN}/${REL_FILE_TO_BASE}.pb.cc"
             "${CMAKE_BINARY_DIR_GEN}/${REL_FILE_TO_BASE}.pb.h"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --cpp_out=${CMAKE_BINARY_DIR_GEN}
	      --proto_path=${PROTO_PATH}
           ${_protobuf_include_path}
           ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${PROTOBUF_PROTOC_EXECUTABLE}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Generating ${FIL}"
      VERBATIM)
  endforeach()
  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()
