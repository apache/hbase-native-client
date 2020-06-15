/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#pragma once

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

namespace hbase {
// main() function intended to be used in tests. This initializes the needed gflags/glog libraries as needed.
#define HBASE_TEST_MAIN() \
  int main(int argc, char** argv) { \
    ::testing::InitGoogleTest(&argc, argv); \
    gflags::ParseCommandLineFlags(&argc, &argv, true);\
    google::InstallFailureSignalHandler();\
    google::InitGoogleLogging(argv[0]);\
    return RUN_ALL_TESTS(); \
  }
} // namespace hbase
