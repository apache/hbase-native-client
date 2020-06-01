# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
set(ZOOKEEPER_INCLUDE_DIRS "${ZOOKEEPER_DIR}/include" CACHE STRING "" FORCE)
set(ZOOKEEPER_LIBRARIES  "${ZOOKEEPER_DIR}/lib/libzookeeper_mt.a" CACHE STRING "" FORCE)	
include_directories(${ZOOKEEPER_INCLUDE_DIRS})
set(ZOOKEEPER_FOUND TRUE CACHE STRING "" FORCE)

mark_as_advanced(
    ZOOKEEPER_FOUND
    ZOOKEEPER_INCLUDE_DIRS
    ZOOKEEPER_LIBRARIES
)
message("-- Zookeeper found, ${ZOOKEEPER_LIBRARIES}")