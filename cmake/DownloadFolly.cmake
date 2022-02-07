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

## Download facebook's folly library. 
## SOURCE_DIR is typically the cmake source directory
function(download_folly SOURCE_DIR BUILD_DIR)

  ExternalProject_Add(
    facebook-folly-proj
    PREFIX "${BUILD_DIR}/dependencies"
    GIT_REPOSITORY "https://github.com/facebook/folly.git"
    GIT_TAG "v2020.05.18.00"
    SOURCE_DIR "${BUILD_DIR}/dependencies/facebook-folly-proj-src"
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
      		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/doubleconversion/local/FindDoubleConversion.cmake" ${BUILD_DIR}/dependencies/facebook-folly-proj-src/CMake
          COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost/local/FindBoost.cmake" ${BUILD_DIR}/dependencies/facebook-folly-proj-src/CMake
    CMAKE_ARGS ${PASSTHROUGH_CMAKE_ARGS}
		  -DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/dependencies/facebook-folly-proj-install
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON
      -DDOUBLE_CONVERSION_ROOT_DIR=${DOUBLE_CONVERSION_ROOT_DIR}
      -DBYPRODUCT_PREFIX=${BYPRODUCT_PREFIX}
      -DBYPRODUCT_SUFFIX=${BYPRODUCT_SUFFIX}
      -DBOOST_ROOT=${BOOST_ROOT}
      -DBOOST_INCLUDEDIR=${BOOST_ROOT}/include
      -DBOOST_LIBRARYDIR=${BOOST_ROOT}/lib
      -DBOOST_LIBRARIES=${BOOST_LIBRARIES}
			"${BUILD_ARGS}"
  )
  set(FOLLY_ROOT_DIR "${BUILD_DIR}/dependencies/facebook-folly-proj-install" CACHE STRING "" FORCE)
endfunction(download_folly) 
