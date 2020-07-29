#!/usr/bin/env bash
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# You'll need a local installation of
# [Apache Yetus' precommit checker](http://yetus.apache.org/documentation/0.1.0/#yetus-precommit)
# to use this personality.
#
# Download from: http://yetus.apache.org/downloads/ . You can either grab the source artifact and
# build from it, or use the convenience binaries provided on that download page.
#
# To run against, e.g. HBASE-15074 you'd then do
# ```bash
# test-patch --personality=bin/hbase-native-client-personality.sh HBASE-15074
# ```
#
# If you want to skip the ~1 hour it'll take to do all the hadoop API checks, use
# ```bash
# test-patch  --plugins=all,-hadoopcheck --personality=dev-support/hbase-personality.sh HBASE-15074
# ````
#
# pass the `--sentinel` flag if you want to allow test-patch to destructively alter local working
# directory / branch in order to have things match what the issue patch requests.

# Plugins enabled.
personality_plugins "compile,unit,cmake,asflicense,github,jira,ctest,htmlout"

if ! declare -f "yetus_info" >/dev/null; then
  function yetus_info
  {
    echo "[$(date) INFO]: $*" 1>&2
  }
fi

# work around yetus overwriting JAVA_HOME from our docker image
function docker_do_env_adds
{
  declare k
  for k in "${DOCKER_EXTRAENVS[@]}"; do
    if [[ "JAVA_HOME" == "${k}" ]]; then
      if [ -n "${JAVA_HOME}" ]; then
        DOCKER_EXTRAARGS+=("--env=JAVA_HOME=${JAVA_HOME}")
      fi
    else
      DOCKER_EXTRAARGS+=("--env=${k}=${!k}")
    fi
  done
  DOCKER_EXTRAARGS+=("-h=securecluster")
}

## @description  Globals specific to this personality
## @audience     private
## @stability    evolving
function personality_globals
{
  # shellcheck disable=SC2034
  SUDO_USER=root
  # shellcheck disable=SC2034
  BUILD_NATIVE=true
  # shellcheck disable=SC2034
  BUILDTOOL=cmake
  # shellcheck disable=SC2034
  # Passed to cmake command using a custom personality.
  CMAKE_ARGS="-DDOWNLOAD_DEPENDENCIES=ON"
  # Passed to make compilation command.
  MAKE_COMPILE_ARGS="-j$(nproc)"
  # shellcheck disable=SC2034
  # Expected by Yetus for compiling non-jvm projects.
  JVM_REQUIRED=false
  #shellcheck disable=SC2034
  PROJECT_NAME=hbase-native-client
  #shellcheck disable=SC2034
  PATCH_BRANCH_DEFAULT=master
  #shellcheck disable=SC2034
  JIRA_ISSUE_RE='^HBASE-[0-9]+$'
  #shellcheck disable=SC2034
  GITHUB_REPO="apache/hbase-native-client"
  # Yetus 0.7.0 enforces limits. Default proclimit is 1000.
  # Up it. See HBASE-19902 for how we arrived at this number.
  #shellcheck disable=SC2034
  PROCLIMIT=10000
  # Override if you want to bump up the memlimit for docker.
  # shellcheck disable=SC2034
  DOCKERMEMLIMIT=4g
}

## @description  Queue up modules for this personality
## @audience     private
## @stability    evolving
## @param        repostatus
## @param        testtype
function personality_modules
{
  local repostatus=$1
  local testtype=$2
  local args
  yetus_info "Personality: ${repostatus} ${testtype}"
  clear_personality_queue
  if [[ "${testtype}" =~ compile ]]; then
    args=${MAKE_COMPILE_ARGS}
    yetus_debug "Appending make compile args ${args}"
  elif [[ "${testtype}" =~ CMakeLists.txt ]]; then
    args=${CMAKE_ARGS}
    yetus_debug "Appending CMake args ${args}"
  fi
  personality_enqueue_module . ${args}
}
