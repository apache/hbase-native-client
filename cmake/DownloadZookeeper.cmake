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

## Download Apache zookeeper 
## SOURCE_DIR is typically the cmake source directory
## BINARY_DIR is the build directory, typically 'build'

#################### ZOOKEEPER

function(download_zookeeper SOURCE_DIR BUILD_DIR)

	ExternalProject_Add(
	  ZooKeeper
	  URL "https://archive.apache.org/dist/zookeeper/zookeeper-3.4.8/zookeeper-3.4.8.tar.gz"
	  SOURCE_DIR "${BUILD_DIR}/dependencies/zookeeper-src"
	  BINARY_DIR ${BUILD_DIR}/dependencies/zookeeper-src/src/c/
	  CONFIGURE_COMMAND ./configure --without-cppunit --prefix=${BUILD_DIR}/dependencies/zookeeper-install
	  PATCH_COMMAND patch ${BUILD_DIR}/dependencies/zookeeper-src/src/c/src/zookeeper.c ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/zookeeper.3.4.x.buf
	  UPDATE_COMMAND ""
	)
	
	add_library(zookeeper STATIC IMPORTED)
	set_target_properties(zookeeper PROPERTIES IMPORTED_LOCATION "${BUILD_DIR}/dependencies/zookeeper-install/lib/libzookeeper_mt.a")
	add_dependencies(zookeeper ZooKeeper)
	
	set(ZOOKEEPER_DIR "${BUILD_DIR}/dependencies/zookeeper-install/" CACHE STRING "" FORCE)
	
		
endfunction(download_zookeeper)