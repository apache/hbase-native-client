#!/usr/bin/env bash
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

set -eo pipefail

if [[ "true" = "${DEBUG}" ]]; then
  set -x
  printenv
fi

declare -i missing_env=0
declare -a required_envs=(
  # these ENV variables define the required API with Jenkinsfile_GitHub
  "ARCHIVE_PATTERN_LIST"
  "BUILD_URL_ARTIFACTS"
  "DOCKERFILE"
  "GITHUB_PASSWORD"
  "GITHUB_USER"
  "PATCHDIR"
  "SOURCEDIR"
  "YETUSDIR"
)
# Validate params
for required_env in "${required_envs[@]}"; do
  if [ -z "${!required_env}" ]; then
    echo "[ERROR] Required environment variable '${required_env}' is not set."
    missing_env=${missing_env}+1
  fi
done

if [ ${missing_env} -gt 0 ]; then
  echo "[ERROR] Please set the required environment variables before invoking. If this error is " \
       "on Jenkins, then please file a JIRA about the error."
  exit 1
fi

# this must be clean for every run
rm -rf "${PATCHDIR}"
mkdir -p "${PATCHDIR}"

# Gather machine information
mkdir "${PATCHDIR}/machine"
"${SOURCEDIR}/bin/jenkins/gather_machine_environment.sh" "${PATCHDIR}/machine"

YETUS_ARGS+=("--archive-list=${ARCHIVE_PATTERN_LIST}")
YETUS_ARGS+=("--ignore-unknown-options=true")
YETUS_ARGS+=("--patch-dir=${PATCHDIR}")
# where the source is located
YETUS_ARGS+=("--basedir=${SOURCEDIR}")
# lots of different output formats
YETUS_ARGS+=("--console-report-file=${PATCHDIR}/console.txt")
YETUS_ARGS+=("--html-report-file=${PATCHDIR}/report.html")
# enable writing back to Github
YETUS_ARGS+=("--github-password=${GITHUB_PASSWORD}")
YETUS_ARGS+=("--github-user=${GITHUB_USER}")
# rsync these files back into the archive dir
YETUS_ARGS+=("--archive-list=${ARCHIVE_PATTERN_LIST}")
# URL for user-side presentation in reports and such to our artifacts
YETUS_ARGS+=("--build-url-artifacts=${BUILD_URL_ARTIFACTS}")
# run in docker mode and specifically point to our
YETUS_ARGS+=("--docker")
YETUS_ARGS+=("--dockerfile=${DOCKERFILE}")

# TODO (HBASE-23900): cannot assume test-patch runs directly from sources
TESTPATCHBIN="${YETUSDIR}/precommit/src/main/shell/test-patch.sh"

if [[ "true" = "${DEBUG}" ]]; then
  YETUS_ARGS=(--debug "${YETUS_ARGS[@]}")
fi

# Sanity checks for downloaded Yetus binaries.
if [ ! -x "${TESTPATCHBIN}" ]; then
  echo "Something is amiss with Yetus download."
  exit 1
fi

# Runs in docker/non-docker mode. In docker mode, we pass the appropriate docker file with
# all the dependencies installed as a part of init.
if [[ "true" = "${RUN_IN_DOCKER}" ]]; then
  YETUS_ARGS=(
    --docker \
    "--dockerfile=${SOURCEDIR}/docker-files/Dockerfile" \
    "${YETUS_ARGS[@]}"
  )
fi

echo "Using YETUS_ARGS: ${YETUS_ARGS[*]}"

# shellcheck disable=SC2068
/bin/bash "${TESTPATCHBIN}" \
    --personality="${SOURCEDIR}/bin/hbase-native-client-personality.sh" \
    --run-tests \
    ${YETUS_ARGS[@]}
