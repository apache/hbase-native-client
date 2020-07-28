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

## Download Cyrus SASL
## SOURCE_DIR is typically the cmake source directory
## BINARY_DIR is the build directory, typically 'build'

function(download_cyrus_sasl SOURCE_DIR BUILD_DIR)
  ExternalProject_Add(
    cyrussasl
    URL "https://github.com/cyrusimap/cyrus-sasl/releases/download/cyrus-sasl-2.1.27/cyrus-sasl-2.1.27.tar.gz"
    PREFIX "${BUILD_DIR}/dependencies"
    SOURCE_DIR "${BUILD_DIR}/dependencies/cyrussasl-src"
    BINARY_DIR ${BUILD_DIR}/dependencies/cyrussasl-src/
    CONFIGURE_COMMAND ./configure --enable-static --with-pic --prefix=${BUILD_DIR}/dependencies/cyrussasl-install
      "CFLAGS=-fPIC"
      "CXXFLAGS=${CMAKE_CXX_FLAGS} -fPIC"
  )
  set(SASL2_DIR "${BUILD_DIR}/dependencies/cyrussasl-install/" CACHE STRING "" FORCE)
endfunction(download_cyrus_sasl)
