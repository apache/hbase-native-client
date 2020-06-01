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

## SOURCE_DIR is typically location of the root pom
## DEST_OUTPUT is the destination output variable
## DEST_RESULT is the destination result variable

function(execute_maven SOURCE_DIR DEST_OUTPUT DEST_RESULT)

    execute_process(
        COMMAND "${MAVEN_EXECUTABLE}" "-q" "package" "-DskipTests" "-Denforcer.skip=true"
        WORKING_DIRECTORY "${SOURCE_DIR}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error_variable
        )
    
    set(${DEST_RESULT} $result PARENT_SCOPE)
    set(${DEST_OUTPUT} $output PARENT_SCOPE)

endfunction(execute_maven) 