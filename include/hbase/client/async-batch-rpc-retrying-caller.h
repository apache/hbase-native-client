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

#include <folly/ExceptionWrapper.h>
#include <folly/Format.h>
#include <folly/Try.h>
#include <folly/futures/Future.h>
#include <folly/futures/Promise.h>
#include <folly/io/IOBuf.h>
#include <folly/io/async/HHWheelTimer.h>
#include <folly/executors/CPUThreadPoolExecutor.h>

#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "hbase/connection/rpc-client.h"
#include "hbase/client/action.h"
#include "hbase/client/async-connection.h"
#include "hbase/client/location-cache.h"
#include "hbase/client/multi-response.h"
#include "hbase/client/region-location.h"
#include "hbase/client/region-request.h"
#include "hbase/client/region-result.h"
#include "hbase/client/request-converter.h"
#include "hbase/client/response-converter.h"
#include "hbase/client/result.h"
#include "hbase/client/row.h"
#include "hbase/client/server-request.h"
#include "hbase/exceptions/exception.h"
#include "client/Client.pb.h"
#include "HBase.pb.h"
#include "hbase/security/user.h"
#include "hbase/utils/connection-util.h"
#include "hbase/utils/sys-util.h"
#include "hbase/utils/time-util.h"

namespace hbase {
/* Equals function for ServerName */
struct ServerNameEquals {
  bool operator()(const std::shared_ptr<pb::ServerName> &lhs,
                  const std::shared_ptr<pb::ServerName> &rhs) const {
    return (lhs->start_code() == rhs->start_code() && lhs->host_name() == rhs->host_name() &&
            lhs->port() == rhs->port());
  }
};

struct ServerNameHash {
  /** hash */
  std::size_t operator()(const std::shared_ptr<pb::ServerName> &sn) const {
    std::size_t h = 0;
    boost::hash_combine(h, sn->start_code());
    boost::hash_combine(h, sn->host_name());
    boost::hash_combine(h, sn->port());
    return h;
  }
};

template <typename REQ, typename RESP>
class AsyncBatchRpcRetryingCaller {
 public:
  using ActionsByServer =
      std::unordered_map<std::shared_ptr<pb::ServerName>, std::shared_ptr<ServerRequest>,
                         ServerNameHash, ServerNameEquals>;
  using ActionsByRegion = ServerRequest::ActionsByRegion;

  AsyncBatchRpcRetryingCaller(std::shared_ptr<AsyncConnection> conn,
                              std::shared_ptr<folly::HHWheelTimer> retry_timer,
                              std::shared_ptr<pb::TableName> table_name,
                              const std::vector<REQ> &actions, std::chrono::nanoseconds pause_ns,
                              int32_t max_attempts, std::chrono::nanoseconds operation_timeout_ns,
                              std::chrono::nanoseconds rpc_timeout_ns,
                              int32_t start_log_errors_count);

  ~AsyncBatchRpcRetryingCaller();

  folly::Future<std::vector<folly::Try<RESP>>> Call();

 private:
  int64_t RemainingTimeNs();

  void LogException(int32_t tries, std::shared_ptr<RegionRequest> region_request,
                    const folly::exception_wrapper &ew,
                    std::shared_ptr<pb::ServerName> server_name);

  void LogException(int32_t tries,
                    const std::vector<std::shared_ptr<RegionRequest>> &region_requests,
                    const folly::exception_wrapper &ew,
                    std::shared_ptr<pb::ServerName> server_name);

  const std::string GetExtraContextForError(std::shared_ptr<pb::ServerName> server_name);

  void AddError(const std::shared_ptr<Action> &action, const folly::exception_wrapper &ew,
                std::shared_ptr<pb::ServerName> server_name);

  void AddError(const std::vector<std::shared_ptr<Action>> &actions,
                const folly::exception_wrapper &ew, std::shared_ptr<pb::ServerName> server_name);

  void FailOne(const std::shared_ptr<Action> &action, int32_t tries,
               const folly::exception_wrapper &ew, int64_t current_time, const std::string extras);

  void FailAll(const std::vector<std::shared_ptr<Action>> &actions, int32_t tries,
               const folly::exception_wrapper &ew, std::shared_ptr<pb::ServerName> server_name);

  void FailAll(const std::vector<std::shared_ptr<Action>> &actions, int32_t tries);

  void AddAction2Error(uint64_t action_index, const ThrowableWithExtraContext &twec);

  void OnError(const ActionsByRegion &actions_by_region, int32_t tries,
               const folly::exception_wrapper &ew, std::shared_ptr<pb::ServerName> server_name);

  void TryResubmit(const std::vector<std::shared_ptr<Action>> &actions, int32_t tries);

  folly::Future<std::vector<folly::Try<std::shared_ptr<RegionLocation>>>> GetRegionLocations(
      const std::vector<std::shared_ptr<Action>> &actions, int64_t locate_timeout_ns);

  void GroupAndSend(const std::vector<std::shared_ptr<Action>> &actions, int32_t tries);

  folly::Future<std::vector<folly::Try<std::unique_ptr<Response>>>> GetMultiResponse(
      const ActionsByServer &actions_by_server);

  void Send(const ActionsByServer &actions_by_server, int32_t tries);

  void OnComplete(const ActionsByRegion &actions_by_region, int32_t tries,
                  const std::shared_ptr<pb::ServerName> server_name,
                  const std::unique_ptr<MultiResponse> multi_results);

  void OnComplete(const std::shared_ptr<Action> &action,
                  const std::shared_ptr<RegionRequest> &region_request, int32_t tries,
                  const std::shared_ptr<pb::ServerName> &server_name,
                  const std::shared_ptr<RegionResult> &region_result,
                  std::vector<std::shared_ptr<Action>> &failed_actions);

 private:
  std::shared_ptr<folly::HHWheelTimer> retry_timer_;
  std::shared_ptr<hbase::AsyncConnection> conn_;
  std::shared_ptr<pb::TableName> table_name_;
  std::vector<std::shared_ptr<Action>> actions_;
  std::chrono::nanoseconds pause_ns_;
  int32_t max_attempts_ = 0;
  std::chrono::nanoseconds operation_timeout_ns_;
  std::chrono::nanoseconds rpc_timeout_ns_;
  int32_t start_log_errors_count_ = 0;

  int64_t start_ns_ = TimeUtil::GetNowNanos();
  int32_t tries_ = 1;
  std::map<uint64_t, folly::Promise<RESP>> action2promises_;
  std::vector<folly::Future<RESP>> action2futures_;
  std::map<uint64_t, std::shared_ptr<std::vector<ThrowableWithExtraContext>>> action2errors_;

  std::shared_ptr<AsyncRegionLocator> location_cache_ = nullptr;
  std::shared_ptr<RpcClient> rpc_client_ = nullptr;
  std::shared_ptr<folly::CPUThreadPoolExecutor> cpu_pool_ = nullptr;

  std::recursive_mutex multi_mutex_;
};
}  // namespace hbase
