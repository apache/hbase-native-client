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

#include <memory>
#include <string>

#include <folly/ExceptionWrapper.h>
#include "HBase.pb.h"
#include "hbase/serde/cell-scanner.h"
#include "hbase/serde/codec.h"

using namespace folly;
// Forward
namespace folly {
class IOBuf;
}
namespace google {
namespace protobuf {
class Message;
}
}

namespace hbase {

/**
 * @brief Class for serializing a deserializing rpc formatted data.
 *
 * RpcSerde is the one stop shop for reading/writing data to HBase daemons.
 * It should throw exceptions if anything goes wrong.
 */
class RpcSerde {
 public:
  RpcSerde();
  /**
   * Constructor assumes the default auth type.
   */
  RpcSerde(std::shared_ptr<Codec> codec);

  /**
   * Destructor. This is provided just for testing purposes.
   */
  virtual ~RpcSerde() = default;

  /**
   * Pase a message in the delimited format.
   *
   * A message in delimited format consists of the following:
   *
   * - a protobuf var int32.
   * - A protobuf object serialized.
   */
  int ParseDelimited(const folly::IOBuf *buf, google::protobuf::Message *msg);

  /**
   * Create a new connection preamble in a new IOBuf.
   */
  static std::unique_ptr<folly::IOBuf> Preamble(bool secure);

  /**
   * Create the header protobuf object and serialize it to a new IOBuf.
   * Header is in the following format:
   *
   * - Big endian length
   * - ConnectionHeader object serialized out.
   */
  std::unique_ptr<folly::IOBuf> Header(const std::string &user);

  /**
   * Take ownership of the passed buffer, and create a CellScanner using the
   * Codec class to parse Cells out of the wire.
   */
  std::unique_ptr<CellScanner> CreateCellScanner(std::unique_ptr<folly::IOBuf> buf, uint32_t offset,
                                                 uint32_t length);

  /**
   * Serialize a request message into a protobuf.
   * Request consists of:
   *
   * - Big endian length
   * - RequestHeader object
   * - The passed in Message object
   */
  std::unique_ptr<folly::IOBuf> Request(const uint32_t call_id, const std::string &method,
                                        const google::protobuf::Message *msg);

  /**
     * Serialize a response message into a protobuf.
     * Request consists of:
     *
     * - Big endian length
     * - ResponseHeader object
     * - The passed in Message object
     */
  std::unique_ptr<folly::IOBuf> Response(const uint32_t call_id,
                                         const google::protobuf::Message *msg);

  /**
   * Serialize a response message into a protobuf.
   * Request consists of:
   *
   * - Big endian length
   * - ResponseHeader object
   * - The passed in hbase::Response object
   */
  std::unique_ptr<folly::IOBuf> Response(const uint32_t call_id,
                                         const google::protobuf::Message *msg,
                                         const folly::exception_wrapper &exception);

  /**
   * Serialize a message in the delimited format.
   * Delimited format consists of the following:
   *
   * - A protobuf var int32
   * - The message object seriailized after that.
   */
  std::unique_ptr<folly::IOBuf> SerializeDelimited(const google::protobuf::Message &msg);

  /**
   * Serilalize a message. This does not add any length prepend.
   */
  std::unique_ptr<folly::IOBuf> SerializeMessage(const google::protobuf::Message &msg);

  /**
   * Prepend a length IOBuf to the given IOBuf chain.
   * This involves no copies or moves of the passed in data.
   */
  std::unique_ptr<folly::IOBuf> PrependLength(std::unique_ptr<folly::IOBuf> msg);

 public:
  static constexpr const char *HBASE_CLIENT_RPC_TEST_MODE = "hbase.client.rpc.test.mode";
  static constexpr const bool DEFAULT_HBASE_CLIENT_RPC_TEST_MODE = false;

 private:
  /* data */
  std::shared_ptr<Codec> codec_;
  std::unique_ptr<pb::VersionInfo> CreateVersionInfo();
};
}  // namespace hbase
