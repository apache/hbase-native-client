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
  set(WANGLE_DOWNLOAD_DIR "${BUILD_DIR}/dependencies/facebook-wangle-proj-download")
  set(WANGLE_SOURCE_DIR "${BUILD_DIR}/dependencies/facebook-wangle-proj-src")
  set(WANGLE_INSTALL_DIR "${BUILD_DIR}/dependencies/facebook-wangle-proj-install")
	ExternalProject_Add(
		facebook-wangle-proj
		GIT_REPOSITORY "https://github.com/facebook/wangle.git"
		GIT_TAG "v2020.05.18.00"
		SOURCE_DIR "${BUILD_DIR}/dependencies/facebook-wangle-proj-src"
		PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake/folly/local/FindFolly.cmake ${WANGLE_SOURCE_DIR}/wangle/cmake/
      		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake/fizz/local/FindFizz.cmake ${WANGLE_SOURCE_DIR}/wangle/cmake/
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost/local/FindBoost.cmake ${WANGLE_SOURCE_DIR}/wangle/cmake/
          COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/cmake/doubleconversion/local/FindDoubleConversion.cmake ${WANGLE_SOURCE_DIR}/wangle/cmake/
      		COMMAND patch ${WANGLE_SOURCE_DIR}/wangle/CMakeLists.txt ${CMAKE_CURRENT_SOURCE_DIR}/cmake/patches/wangle.v2020.05.18.00.cmake
		INSTALL_DIR "${WANGLE_INSTALL_DIR}"
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DBUILD_EXAMPLES=OFF -DCMAKE_CROSSCOMPILING=ON -DBUILD_TESTS=OFF -DFIZZ_ROOT_DIR=${FIZZ_ROOT_DIR} -DFOLLY_ROOT_DIR=${FOLLY_ROOT_DIR} -DDOUBLE_CONVERSION_ROOT_DIR=${DOUBLE_CONVERSION_ROOT_DIR} -DBYPRODUCT_PREFIX=${BYPRODUCT_PREFIX}  -DCMAKE_INSTALL_PREFIX:PATH=${WANGLE_INSTALL_DIR}
        	${WANGLE_SOURCE_DIR}/wangle # Tell CMake to use subdirectory as source.
		)

	set(WANGLE_ROOT_DIR "${WANGLE_INSTALL_DIR}" CACHE STRING "" FORCE)
endfunction(download_wangle) 
