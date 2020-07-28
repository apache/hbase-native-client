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

## Download Boost. 
## SOURCE_DIR is typically the cmake source directory
## BINARY_DIR is the build directory, typically 'build'
## Sets BOOST_ROOT, search prefix for FindBoost.

function(download_boost SOURCE_DIR BUILD_DIR BOOST_LIBS)
  set(BOOST_DOWNLOAD_DIR "${BUILD_DIR}/dependencies/boost-download")
  set(BOOST_SOURCE_DIR "${BUILD_DIR}/dependencies/boost-src")
  set(BOOST_INSTALL_DIR "${BUILD_DIR}/dependencies/boost-install")

  set(CFLAGS "-fPIC")
  set(CXXFLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++14")

  # Only compile and install the needed libs.
  set(LIBS_TO_COMPILE "")
  foreach(lib ${BOOST_LIBS})
    string(APPEND LIBS_TO_COMPILE --with-${lib} " ")
  endforeach()

  separate_arguments(BUILD_CMD UNIX_COMMAND
    "./b2 cflags='${CFLAGS}' cxxflags='${CXXFLAGS}' variant=release link=static threading=multi ${LIBS_TO_COMPILE} install")

  ExternalProject_Add(boost
     URL "https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz"
     PREFIX "${BUILD_DIR}/dependencies"
     DOWNLOAD_DIR ${BOOST_DOWNLOAD_DIR}
     BUILD_IN_SOURCE true
     SOURCE_DIR ${BOOST_SOURCE_DIR}
     INSTALL_DIR ${BOOST_INSTALL_DIR}
     CONFIGURE_COMMAND ./bootstrap.sh --prefix=${BOOST_INSTALL_DIR}
     BUILD_COMMAND ${BUILD_CMD}
     INSTALL_COMMAND ""
  )
  set(BOOST_ROOT ${BOOST_INSTALL_DIR} PARENT_SCOPE)
  set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include PARENT_SCOPE)
  set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib PARENT_SCOPE)
  set(Boost_FIND_COMPONENTS ${BOOST_LIBS} PARENT_SCOPE)
endfunction(download_boost) 
