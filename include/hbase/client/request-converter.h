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
#include <vector>
#include <folly/logging/Logger.h>
#include "hbase/connection/request.h"
#include "hbase/client/action.h"
#include "hbase/client/append.h"
#include "hbase/client/cell.h"
#include "hbase/client/delete.h"
#include "hbase/client/get.h"
#include "hbase/client/increment.h"
#include "hbase/client/mutation.h"
#include "hbase/client/put.h"
#include "hbase/client/region-request.h"
#include "hbase/client/scan.h"
#include "hbase/client/server-request.h"
#include "client/Client.pb.h"
#include "HBase.pb.h"

using MutationType = hbase::pb::MutationProto_MutationType;
using DeleteType = hbase::pb::MutationProto_DeleteType;

namespace hbase {

using ActionsByRegion = ServerRequest::ActionsByRegion;
/**
 * RequestConverter class
 * This class converts a Client side Get, Scan, Mutate operation to corresponding PB message.
 */
class RequestConverter {
 public:
  ~RequestConverter();

  /**
   * @brief Returns a Request object comprising of PB GetRequest created using
   * passed 'get'
   * @param get - Get object used for creating GetRequest
   * @param region_name - table region
   */
  static std::unique_ptr<Request> ToGetRequest(const Get &get, const std::string &region_name);

  /**
   * @brief Returns a Request object comprising of PB ScanRequest created using
   * passed 'scan'
   * @param scan - Scan object used for creating ScanRequest
   * @param region_name - table region
   */
  static std::unique_ptr<Request> ToScanRequest(const Scan &scan, const std::string &region_name);

  static std::unique_ptr<Request> ToScanRequest(const Scan &scan, const std::string &region_name,
                                                int32_t num_rows, bool close_scanner);

  static std::unique_ptr<Request> ToScanRequest(int64_t scanner_id, int32_t num_rows,
                                                bool close_scanner);

  static std::unique_ptr<Request> ToScanRequest(int64_t scanner_id, int32_t num_rows,
                                                bool close_scanner, int64_t next_call_seq_id,
                                                bool renew);

  static std::unique_ptr<Request> ToMultiRequest(const ActionsByRegion &region_requests);

  static std::unique_ptr<Request> DeleteToMutateRequest(const Delete &del,
                                                        const std::string &region_name);

  static std::unique_ptr<Request> ToMutateRequest(const Put &put, const std::string &region_name);

  static std::unique_ptr<Request> CheckAndPutToMutateRequest(
      const std::string &row, const std::string &family, const std::string &qualifier,
      const std::string &value, const pb::CompareType compare_op, const hbase::Put &put,
      const std::string &region_name);

  static std::unique_ptr<Request> CheckAndDeleteToMutateRequest(
      const std::string &row, const std::string &family, const std::string &qualifier,
      const std::string &value, const pb::CompareType compare_op, const hbase::Delete &del,
      const std::string &region_name);

  static std::unique_ptr<Request> IncrementToMutateRequest(const Increment &incr,
                                                           const std::string &region_name);

  static std::unique_ptr<pb::MutationProto> ToMutation(const MutationType type,
                                                       const Mutation &mutation,
                                                       const int64_t nonce);

  static std::unique_ptr<Request> AppendToMutateRequest(const Append &append,
                                                        const std::string &region_name);

 private:
  // Constructor not required. We have all static methods to create PB requests.
  RequestConverter();

  /**
   * @brief fills region_specifier with region values.
   * @param region_name - table region
   * @param region_specifier - RegionSpecifier to be filled and passed in PB
   * Request.
   */
  static void SetRegion(const std::string &region_name, pb::RegionSpecifier *region_specifier);
  static std::unique_ptr<hbase::pb::Get> ToGet(const Get &get);
  static std::unique_ptr<hbase::pb::Scan> ToScan(const Scan &scan);
  static DeleteType ToDeleteType(const CellType type);
  static bool IsDelete(const CellType type);
  static void SetCommonScanRequestFields(std::shared_ptr<hbase::pb::ScanRequest>, bool renew);
};

} /* namespace hbase */
