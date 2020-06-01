# Licensed to the Apache Software Foundation (ASF) under one
#
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
#

# Stubs to allow us to find folly libs

set(WANGLE_FOUND "true" CACHE STRING "" FORCE)
set(WANGLE_ROOT_DIR "${WANGLE_ROOT_DIR}" CACHE STRING "" FORCE)
set(WANGLE_INCLUDE_DIR "${WANGLE_ROOT_DIR}/include" CACHE STRING "" FORCE)
set(WANGLE_LIBRARIES "${WANGLE_ROOT_DIR}/lib/${BYPRODUCT_PREFIX}wangle${BYPRODUCT_SUFFIX}" CACHE STRING "" FORCE)

mark_as_advanced(
    WANGLE_ROOT_DIR
    WANGLE_LIBRARIES
    WANGLE_INCLUDE_DIR
)
  message("-- Wangle found, ${WANGLE_LIBRARIES}")