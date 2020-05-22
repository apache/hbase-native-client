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

## Download Protobuf 
## SOURCE_DIR is typically the cmake source directory
## BINARY_DIR is the build directory, typically 'build'

#################### PROTOBUF

function(download_protobuf SOURCE_DIR BINARY_DIR)


	ExternalProject_Add(
	  Protobuf
	  GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
	  GIT_TAG "3.5.1.1"
	  BUILD_IN_SOURCE true
	  SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf-src"
	  CONFIGURE_COMMAND ./autogen.sh
  			 COMMAND ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf 
  			"CFLAGS=-fPIC"
  			"CXXFLAGS=${CMAKE_CXX_FLAGS} -fPIC"
	  UPDATE_COMMAND ""
	  )

	
	set(PROTOBUF_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf/" CACHE STRING "" FORCE)
	
	add_library(protobuf STATIC IMPORTED)
	set_target_properties(protobuf PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf/lib/libprotobuf.a" )
	set(PROTOBUF_LIBS "${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf/lib/libprotobuf.a" "${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf/lib/libprotoc.a" CACHE STRING "" FORCE)
	set(PROTOBUF_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/dependencies/protobuf/include" CACHE STRING "" FORCE)
	add_dependencies(protobuf Protobuf)
	set(PROTOBUF_FOUND TRUE CACHE STRING "" FORCE)
	
		
endfunction(download_protobuf)

