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

# Stubs to allow us to find folly libs

set(FOLLY_FOUND "true" CACHE STRING "" FORCE)
set(FOLLY_INCLUDE_DIR "${FOLLY_ROOT_DIR}/include" CACHE STRING "" FORCE)
## Given that folly is an older dependency, and the way it is built has evolved, newer
## versions of folly won't require an SO. For now it is far easier to link against the .so (BYPRODUCT_SHARED_SUFFIX)
set(FOLLY_LIBRARIES "${FOLLY_ROOT_DIR}/lib/${BYPRODUCT_PREFIX}folly${BYPRODUCT_SUFFIX}" CACHE STRING "" FORCE)



mark_as_advanced(
    FOLLY_ROOT_DIR
    FOLLY_INCLUDE_DIR
    FOLLY_LIBRARIES
)
message(STATUS "FOLLY found, ${FOLLY_LIBRARIES}")