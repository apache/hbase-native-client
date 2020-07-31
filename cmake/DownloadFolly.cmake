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
## BUILD_DIR is the build directory, typically 'build'

function(download_folly SOURCE_DIR BUILD_DIR)
   
  if (DOWNLOAD_DEPENDENCIES)
    # Add custom boost include and lib paths.
    set(CFLAGS "-fPIC -I${BOOST_ROOT}/include -lboost_context -lboost_coroutine -l${CMAKE_DL_LIBS}")
    set(CXXFLAGS "${CMAKE_CXX_FLAGS} -fPIC -I${BOOST_ROOT}/include -lboost_context -lboost_coroutine -l${CMAKE_DL_LIBS}")
    set(LDFLAGS "-L${BOOST_ROOT}/lib")
    set(CONFIGURE_CMD ./configure --prefix=${BUILD_DIR}/dependencies/facebook-folly-proj-install
      --with-boost-libdir=${BOOST_ROOT}/lib CFLAGS=${CFLAGS} CXXFLAGS=${CXXFLAGS} LDFLAGS=${LDFLAGS})
  else()
    set(CFLAGS "-fPIC -lboost_context -lboost_coroutine -l${CMAKE_DL_LIBS}")
    set(CXXFLAGS "${CMAKE_CXX_FLAGS} -fPIC -lboost_context -lboost_coroutine -l${CMAKE_DL_LIBS}")
    set(CONFIGURE_CMD ./configure --prefix=${BUILD_DIR}/dependencies/facebook-folly-proj-install CFLAGS=${CFLAGS} CXXFLAGS=${CXXFLAGS})
  endif()

  ExternalProject_Add(
      facebook-folly-proj
      # TODO: Source version information from cmake file.
      URL "https://github.com/facebook/folly/archive/v2017.09.04.00.tar.gz"
      PREFIX "${BUILD_DIR}/dependencies"
      SOURCE_DIR "${BUILD_DIR}/dependencies/facebook-folly-proj-src"
      BINARY_DIR ${BUILD_DIR}/dependencies/facebook-folly-proj-src/folly
      CONFIGURE_COMMAND autoreconf -ivf COMMAND ${CONFIGURE_CMD}
      UPDATE_COMMAND ""
  )
  set(FOLLY_ROOT_DIR "${BUILD_DIR}/dependencies/facebook-folly-proj-install" CACHE STRING "" FORCE)
endfunction(download_folly) 
