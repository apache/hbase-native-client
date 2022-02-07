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

#include "hbase/test-util/mini-cluster-util.h"
#include <folly/GLog.h>
#include <string.h>

#include <folly/Format.h>

#include "hbase/client/zk-util.h"

using hbase::MiniClusterUtility;
using folly::Random;

std::string MiniClusterUtility::RandString(int len) {
  // Create the whole string.
  // Filling everything with z's
  auto s = std::string(len, 'z');

  // Now pick a bunch of random numbers
  for (int i = 0; i < len; i++) {
    // use Folly's random to get the numbers
    // as I don't want to have to learn
    // all the cpp rand invocation magic.
    auto r = Random::rand32('a', 'z');
    // Cast that to ascii.
    s[i] = static_cast<char>(r);
  }
  return s;
}

MiniClusterUtility::MiniClusterUtility() : temp_dir_(MiniClusterUtility::RandString()) {}

void MiniClusterUtility::StartMiniCluster(int32_t num_region_servers) {
  mini_ = std::make_unique<MiniCluster>();
  mini_->StartCluster(num_region_servers);

  conf()->Set(ZKUtil::kHBaseZookeeperQuorum_, mini_->GetConfValue(ZKUtil::kHBaseZookeeperQuorum_));
  conf()->Set(ZKUtil::kHBaseZookeeperClientPort_,
              mini_->GetConfValue(ZKUtil::kHBaseZookeeperClientPort_));
}

void MiniClusterUtility::StopMiniCluster() { mini_->StopCluster(); }

void MiniClusterUtility::CreateTable(const std::string &table, const std::string &family) {
  mini_->CreateTable(table, family);
}

void MiniClusterUtility::CreateTable(const std::string &table, const std::vector<std::string> &families) {
  mini_->CreateTable(table, families);
}

void MiniClusterUtility::CreateTable(const std::string &table, const std::string &family,
                                     const std::vector<std::string> &keys) {
  mini_->CreateTable(table, family, keys);
}

void MiniClusterUtility::CreateTable(const std::string &table, const std::vector<std::string> &families,
                                     const std::vector<std::string> &keys) {
  mini_->CreateTable(table, families, keys);
}

void MiniClusterUtility::MoveRegion(const std::string &region, const std::string &server) {
  mini_->MoveRegion(region, server);
}

void MiniClusterUtility::StartStandAloneInstance() {
  auto p = temp_dir_.path().string();
  auto cmd = std::string{"bin/start-local-hbase.sh " + p};
  auto res_code = std::system(cmd.c_str());
  CHECK_EQ(res_code, 0);
}

void MiniClusterUtility::StopStandAloneInstance() {
  auto res_code = std::system("bin/stop-local-hbase.sh");
  CHECK_EQ(res_code, 0);
}

void MiniClusterUtility::RunShellCmd(const std::string &command) {
  auto cmd_string = folly::sformat("echo \"{}\" | ../bin/hbase shell", command);
  auto res_code = std::system(cmd_string.c_str());
  CHECK_EQ(res_code, 0);
}
