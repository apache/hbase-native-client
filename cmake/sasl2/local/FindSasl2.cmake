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

set(SASL2_FOUND "true" CACHE STRING "" FORCE)
set(SASL2_INCLUDE_DIR "${SASL2_DIR}/include" CACHE STRING "" FORCE)
set(SASL_INCLUDE_DIRS "${SASL2_INCLUDE_DIR}" CACHE STRING "" FORCE)
set(SASL2_LIBRARIES "${SASL2_DIR}/lib/sasl2/${BYPRODUCT_PREFIX}gs2${BYPRODUCT_SUFFIX}" "${SASL2_DIR}/lib/sasl2/${BYPRODUCT_PREFIX}gssapiv2${BYPRODUCT_SUFFIX}" "${SASL2_DIR}/lib/${BYPRODUCT_PREFIX}sasl2${BYPRODUCT_SUFFIX}" CACHE STRING "" FORCE)
set(SASL_LIBS "${SASL2_LIBRARIES}" CACHE STRING "" FORCE)

mark_as_advanced(
    SASL2_FOUND
    WANGLE_LIBRARIES
    SASL2_INCLUDE_DIR
    SASL_INCLUDE_DIRS
    SASL2_LIBRARIES
    SASL_LIBS
)
  message(STATUS "SASL2 found, ${SASL2_LIBRARIES}")