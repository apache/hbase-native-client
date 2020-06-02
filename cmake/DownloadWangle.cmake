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

## Download facebook's wangle library. 
## SOURCE_DIR is typically the cmake source directory
## BINARY_DIR is the build directory, typically 'build'

function(download_wangle SOURCE_DIR BUILD_DIR)

	if (BUILD_LOCAL_DEPENDENCIES)
		SET (PATCH_FOLLY ${CMAKE_COMMAND} -E copy
      		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/folly/local/FindFolly.cmake" ${BUILD_DIR}/facebook-wangle-proj-prefix/src/facebook-wangle-proj/wangle/cmake )
	else()
		SET (PATCH_FOLLY "")
	endif() 
	
	ExternalProject_Add(
		facebook-wangle-proj
		URL "https://github.com/facebook/wangle/archive/v2017.09.04.00.tar.gz"
		PATCH_COMMAND ${PATCH_FOLLY}
		INSTALL_DIR "${BUILD_DIR}/dependencies/facebook-wangle-proj-install"
		 CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_EXAMPLES=OFF -DCMAKE_CROSSCOMPILING=ON -DBUILD_TESTS=OFF -DFOLLY_ROOT_DIR=${FOLLY_ROOT_DIR} -DCMAKE_INSTALL_PREFIX:PATH=${BUILD_DIR}/dependencies/facebook-wangle-proj-install
        	${BUILD_DIR}/facebook-wangle-proj-prefix/src/facebook-wangle-proj/wangle # Tell CMake to use subdirectory as source.
		)


	set(WANGLE_ROOT_DIR "${BUILD_DIR}/dependencies/facebook-wangle-proj-install" CACHE STRING "" FORCE)
	
endfunction(download_wangle) 