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

#include "hbase/client/table.h"

#include <chrono>
#include <limits>
#include <utility>
#include <vector>

#include "hbase/client/async-connection.h"
#include "hbase/client/async-table-result-scanner.h"
#include "hbase/client/request-converter.h"
#include "hbase/client/response-converter.h"
#include "hbase/if/client/Client.pb.h"
#include "hbase/security/user.h"
#include "hbase/serde/server-name.h"
#include "hbase/utils/time-util.h"

using hbase::pb::TableName;
using hbase::security::User;
using std::chrono::milliseconds;

namespace hbase {

Table::Table(const TableName &table_name, std::shared_ptr<AsyncConnection> async_connection)
    : table_name_(std::make_shared<TableName>(table_name)),
      async_connection_(async_connection),
      conf_(async_connection->conf()) {
  async_table_ = std::make_unique<RawAsyncTable>(table_name_, async_connection);
}

Table::~Table() {}

std::shared_ptr<hbase::Result> Table::Get(const hbase::Get &get) {
  return async_table_->Get(get).get(operation_timeout());
}

std::shared_ptr<ResultScanner> Table::Scan(const hbase::Scan &scan) {
  auto max_cache_size = ResultSize2CacheSize(
      scan.MaxResultSize() > 0 ? scan.MaxResultSize()
                               : async_connection_->connection_conf()->scanner_max_result_size());
  auto scanner = std::make_shared<AsyncTableResultScanner>(max_cache_size);
  async_table_->Scan(scan, scanner);
  return scanner;
}

int64_t Table::ResultSize2CacheSize(int64_t max_results_size) const {
  // * 2 if possible
  return max_results_size > (std::numeric_limits<int64_t>::max() / 2) ? max_results_size
                                                                      : max_results_size * 2;
}

void Table::Put(const hbase::Put &put) {
  async_table_->Put(put).get(operation_timeout());
}

bool Table::CheckAndPut(const std::string &row, const std::string &family,
                        const std::string &qualifier, const std::string &value,
                        const hbase::Put &put, const pb::CompareType &compare_op) {
  return async_table_->CheckAndPut(row, family, qualifier, value, put, compare_op).get(operation_timeout());
}

bool Table::CheckAndDelete(const std::string &row, const std::string &family,
                           const std::string &qualifier, const std::string &value,
                           const hbase::Delete &del, const pb::CompareType &compare_op) {
  return async_table_->CheckAndDelete(row, family, qualifier, value, del, compare_op).get(operation_timeout());
}

void Table::Delete(const hbase::Delete &del) {
  async_table_->Delete(del).get(operation_timeout());
}

std::shared_ptr<hbase::Result> Table::Increment(const hbase::Increment &increment) {
  return async_table_->Increment(increment).get(operation_timeout());
}

std::shared_ptr<hbase::Result> Table::Append(const hbase::Append &append) {
  return async_table_->Append(append).get(operation_timeout());
}

milliseconds Table::operation_timeout() const {
  return TimeUtil::ToMillis(async_connection_->connection_conf()->operation_timeout());
}

void Table::Close() { async_table_->Close(); }

std::shared_ptr<RegionLocation> Table::GetRegionLocation(const std::string &row) {
  return async_connection_->region_locator()->LocateRegion(*table_name_, row).get();
}

std::vector<std::shared_ptr<hbase::Result>> Table::Get(const std::vector<hbase::Get> &gets) {
  auto tresults = async_table_->Get(gets).get(operation_timeout());
  std::vector<std::shared_ptr<hbase::Result>> results{};
  uint32_t num = 0;
  for (auto tresult : tresults) {
    if (tresult.hasValue()) {
      results.push_back(tresult.value());
    } else if (tresult.hasException()) {
      LOG(ERROR) << "Caught exception:- " << tresult.exception().what() << " for "
                 << gets[num++].row();
      throw tresult.exception();
    }
  }
  return results;
}

void Table::Put(const std::vector<hbase::Put> &puts) {
  auto tresults = async_table_->Put(puts).get(operation_timeout());
  uint32_t num = 0;
  for (auto tresult : tresults) {
    if (tresult.hasException()) {
      LOG(ERROR) << "Caught exception:- " << tresult.exception().what() << " for "
                 << puts[num++].row();
      throw tresult.exception();
    }
  }
  return;
}

} /* namespace hbase */
