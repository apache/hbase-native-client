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

#include <folly/SharedMutex.h>
#include <boost/functional/hash.hpp>
#include <chrono>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "hbase/connection/connection-factory.h"
#include "hbase/connection/connection-id.h"
#include "hbase/connection/rpc-connection.h"
#include "hbase/connection/service.h"
#include "hbase/if/HBase.pb.h"

namespace hbase {

/**
 * @brief Connection pooling for HBase rpc connection.
 *
 * This is a thread safe connection pool. It allows getting
 * a shared rpc connection to HBase servers by connection id.
 */
class ConnectionPool {
 public:
  /** Create connection pool wit default connection factory */
  ConnectionPool(std::shared_ptr<folly::IOThreadPoolExecutor> io_executor,
                 std::shared_ptr<folly::CPUThreadPoolExecutor> cpu_executor,
                 std::shared_ptr<Codec> codec, std::shared_ptr<Configuration> conf,
                 std::chrono::nanoseconds connect_timeout = std::chrono::nanoseconds(0));

  /**
   * Constructor that allows specifiying the connetion factory.
   * This is useful for testing.
   */
  explicit ConnectionPool(std::shared_ptr<ConnectionFactory> cf);

  /**
   * Destructor.
   * All connections will be close.
   * All connections will be released
   */
  ~ConnectionPool();

  /**
   * Get a connection to the server name. Start time is ignored.
   * This can be a blocking operation for a short time.
   */
  std::shared_ptr<RpcConnection> GetConnection(std::shared_ptr<ConnectionId> remote_id);

  /**
   * Close/remove a connection.
   */
  void Close(std::shared_ptr<ConnectionId> remote_id);

  /**
   * Close the Connection Pool
   */
  void Close();

 private:
  std::shared_ptr<RpcConnection> GetCachedConnection(std::shared_ptr<ConnectionId> remote_id);
  std::shared_ptr<RpcConnection> GetNewConnection(std::shared_ptr<ConnectionId> remote_id);
  std::unordered_map<std::shared_ptr<ConnectionId>, std::shared_ptr<RpcConnection>,
                     ConnectionIdHash, ConnectionIdEquals>
      connections_;
  folly::SharedMutexWritePriority map_mutex_;
  std::shared_ptr<ConnectionFactory> cf_;
  std::shared_ptr<Configuration> conf_;
};

}  // namespace hbase
