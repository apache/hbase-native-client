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

## Download facebook's fizz library. 
## SOURCE_DIR is typically the cmake source directory
## BINARY_DIR is the build directory, typically 'build'

function(download_fizz SOURCE_DIR BUILD_DIR)

	
	ExternalProject_Add(
		facebook-fizz-proj
		GIT_REPOSITORY "https://github.com/facebookincubator/fizz.git"
		GIT_TAG "v2020.05.18.00"
		SOURCE_DIR "${BUILD_DIR}/dependencies/facebook-fizz-proj-src"
		PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/folly/local/FindFolly.cmake" ${BUILD_DIR}/dependencies/facebook-fizz-proj-src/fizz/cmake/
      		COMMAND patch ${BUILD_DIR}/dependencies/facebook-fizz-proj-src/fizz/CMakeLists.txt "${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/fizz.v2020.05.18.00.cmake" 
		INSTALL_DIR "${BUILD_DIR}/dependencies/facebook-fizz-proj-install"
		 CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_EXAMPLES=OFF -DCMAKE_CROSSCOMPILING=ON -DBUILD_TESTS=OFF -DFOLLY_ROOT_DIR=${FOLLY_ROOT_DIR} -DCMAKE_INSTALL_PREFIX:PATH=${BUILD_DIR}/dependencies/facebook-fizz-proj-install
        	${BUILD_DIR}/dependencies/facebook-fizz-proj-src/fizz # Tell CMake to use subdirectory as source.
		)


	set(FIZZ_ROOT_DIR "${BUILD_DIR}/dependencies/facebook-fizz-proj-install" CACHE STRING "" FORCE)
	
endfunction(download_fizz) 