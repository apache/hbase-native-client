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

# Stubs to allow us to find Boost libs

set(Boost_INCLUDE_DIRS "${BOOST_ROOT}/include" CACHE STRING "" FORCE)
set(Boost_INCLUDE_DIR "${BOOST_ROOT}/include" CACHE STRING "" FORCE)

set(Boost_LIBRARIES "" CACHE STRING "" FORCE)
foreach(COMPONENT ${Boost_FIND_COMPONENTS})
  list(APPEND Boost_LIBRARIES "${BOOST_ROOT}/lib/${BYPRODUCT_PREFIX}boost_${COMPONENT}${BYPRODUCT_SUFFIX}")
endforeach()

set(Boost_FOUND "true" CACHE STRING "" FORCE)

mark_as_advanced(
    Boost_FOUND
    Boost_INCLUDE_DIR
    Boost_INCLUDE_DIRS
    Boost_LIBRARIES
)
message("-- Boost found, ${Boost_LIBRARIES}")
