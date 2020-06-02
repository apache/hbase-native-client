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

	
	ExternalProject_Add(
		facebook-folly-proj
		URL "https://github.com/facebook/folly/archive/v2017.09.04.00.tar.gz"
		#GIT_REPOSITORY "https://github.com/facebook/folly.git"
		#GIT_TAG "v2017.09.04.00"
		SOURCE_DIR "${BUILD_DIR}/dependencies/facebook-folly-proj-src"
		BINARY_DIR ${BUILD_DIR}/dependencies/facebook-folly-proj-src/folly
		CONFIGURE_COMMAND autoreconf -ivf
			COMMAND ./configure  --prefix=${BUILD_DIR}/dependencies/facebook-folly-proj-install
			"CFLAGS=-fPIC -lboost_context -lboost_coroutine -ldl" ## this version of folly does not support cmake so we must pass args manually
  			"CXXFLAGS=${CMAKE_CXX_FLAGS} -fPIC -lboost_context -lboost_coroutine -ldl" ## this version of folly does not support cmake so we must pass args manually
		UPDATE_COMMAND ""
		)


	set(FOLLY_ROOT_DIR "${BUILD_DIR}/dependencies/facebook-folly-proj-install" CACHE STRING "" FORCE)
	
endfunction(download_folly) 