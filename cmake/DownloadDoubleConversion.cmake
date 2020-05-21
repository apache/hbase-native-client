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
function(download_doubleconversion SOURCE_DIR BUILD_DIR)
	ExternalProject_Add(
		doubleconversion-proj
		GIT_REPOSITORY "https://github.com/google/double-conversion.git"
		GIT_TAG "master"
		SOURCE_DIR "${BUILD_DIR}/dependencies/doubleconversion-proj-src"
		CMAKE_ARGS ${PASSTHROUGH_CMAKE_ARGS}
				"-DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/dependencies/doubleconversion-proj-install"
				-DCMAKE_POSITION_INDEPENDENT_CODE=ON
				"${BUILD_ARGS}"
		)


	set(DOUBLE_CONVERSION_ROOT_DIR "${BUILD_DIR}/dependencies/doubleconversion-proj-install" CACHE STRING "" FORCE)
endfunction(download_doubleconversion) 
