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

#include "hbase/client/region-result.h"
#include <glog/logging.h>
#include <stdexcept>

using hbase::pb::RegionLoadStats;

namespace hbase {

RegionResult::RegionResult() {}

RegionResult::~RegionResult() {}

void RegionResult::AddResultOrException(int32_t index, std::shared_ptr<hbase::Result> result,
                                        std::shared_ptr<folly::exception_wrapper> exc) {
  auto index_found = result_or_excption_.find(index);
  if (index_found == result_or_excption_.end()) {
    result_or_excption_[index] = std::make_tuple(result ? result : nullptr, exc ? exc : nullptr);
  } else {
    throw std::runtime_error("Index " + std::to_string(index) +
                             " already set with ResultOrException");
  }
}

void RegionResult::set_stat(std::shared_ptr<RegionLoadStats> stat) { stat_ = stat; }

int RegionResult::ResultOrExceptionSize() const { return result_or_excption_.size(); }

std::shared_ptr<ResultOrExceptionTuple> RegionResult::ResultOrException(int32_t index) const {
  return std::make_shared<ResultOrExceptionTuple>(result_or_excption_.at(index));
}

const std::shared_ptr<RegionLoadStats>& RegionResult::stat() const { return stat_; }

} /* namespace hbase */
