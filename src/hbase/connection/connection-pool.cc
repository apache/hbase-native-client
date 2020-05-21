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

#include "hbase/connection/connection-pool.h"

#include <folly/Conv.h>
#include <folly/logging/Logger.h>
#include <wangle/service/Service.h>

#include <memory>
#include <string>
#include <utility>

using std::chrono::nanoseconds;

namespace hbase {

ConnectionPool::ConnectionPool(std::shared_ptr<folly::IOThreadPoolExecutor> io_executor,
                               std::shared_ptr<folly::CPUThreadPoolExecutor> cpu_executor,
                               std::shared_ptr<Codec> codec, std::shared_ptr<Configuration> conf,
                               nanoseconds connect_timeout)
    : cf_(std::make_shared<ConnectionFactory>(io_executor, cpu_executor, codec, conf,
                                              connect_timeout)),
      connections_(),
      map_mutex_(),
      conf_(conf) {}
ConnectionPool::ConnectionPool(std::shared_ptr<ConnectionFactory> cf)
    : cf_(cf), connections_(), map_mutex_() {}

ConnectionPool::~ConnectionPool() {}

std::shared_ptr<RpcConnection> ConnectionPool::GetConnection(
    std::shared_ptr<ConnectionId> remote_id) {
  // Try and get th cached connection.
  auto found_ptr = GetCachedConnection(remote_id);

  // If there's no connection then create it.
  if (found_ptr == nullptr) {
    found_ptr = GetNewConnection(remote_id);
  }
  return found_ptr;
}

std::shared_ptr<RpcConnection> ConnectionPool::GetCachedConnection(
    std::shared_ptr<ConnectionId> remote_id) {
  folly::SharedMutexWritePriority::ReadHolder holder(map_mutex_);
  auto found = connections_.find(remote_id);
  if (found == connections_.end()) {
    return nullptr;
  }
  return found->second;
}

std::shared_ptr<RpcConnection> ConnectionPool::GetNewConnection(
    std::shared_ptr<ConnectionId> remote_id) {
  // Grab the upgrade lock. While we are double checking other readers can
  // continue on
  folly::SharedMutexWritePriority::UpgradeHolder u_holder{map_mutex_};

  // Now check if someone else created the connection before we got the lock
  // This is safe since we hold the upgrade lock.
  // upgrade lock is more power than the reader lock.
  auto found = connections_.find(remote_id);
  if (found != connections_.end() && found->second != nullptr) {
    return found->second;
  } else {
    // Yeah it looks a lot like there's no connection
    folly::SharedMutexWritePriority::WriteHolder w_holder{std::move(u_holder)};

    // Make double sure there are not stale connections hanging around.
    connections_.erase(remote_id);

    /* create new connection */
    auto connection = std::make_shared<RpcConnection>(remote_id, cf_);

    connections_.insert(std::make_pair(remote_id, connection));

    return connection;
  }
}

void ConnectionPool::Close(std::shared_ptr<ConnectionId> remote_id) {
  folly::SharedMutexWritePriority::WriteHolder holder{map_mutex_};
  DLOG(INFO) << "Closing RPC Connection to host:" << remote_id->host()
             << ", port:" << folly::to<std::string>(remote_id->port());

  auto found = connections_.find(remote_id);
  if (found == connections_.end() || found->second == nullptr) {
    return;
  }
  found->second->Close();
  connections_.erase(found);
}

void ConnectionPool::Close() {
  folly::SharedMutexWritePriority::WriteHolder holder{map_mutex_};
  for (auto &item : connections_) {
    auto &con = item.second;
    con->Close();
  }
  connections_.clear();
}
}  // namespace hbase
