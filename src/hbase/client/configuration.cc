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

#include "hbase/client/configuration.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include <glog/logging.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace hbase {

Configuration::Configuration() : hb_property_() {}

Configuration::Configuration(ConfigMap &config_map) : hb_property_(std::move(config_map)) {}

Configuration::~Configuration() {}

size_t Configuration::IsSubVariable(const std::string &expr, std::string &sub_variable) const {
  size_t start_pos = expr.find("${");
  if (std::string::npos != start_pos) {
    size_t pos_next = expr.find("}", start_pos + 1);
    if (std::string::npos != pos_next) {
      sub_variable = expr.substr(start_pos + 2, pos_next - (start_pos + 2));
    }
  }
  return start_pos;
}

std::string Configuration::SubstituteVars(const std::string &expr) const {
  if (0 == expr.size()) return expr;

  std::string eval(expr);
  std::string value_to_be_replaced("");
  std::string var("");
  for (int i = 0; i < kMaxSubsts; i++) {
    var = "";
    size_t start_pos = IsSubVariable(eval, var);
    if (start_pos != std::string::npos) {
      // We are blindly checking for environment property at first.
      // If we don't get any value from GetEnv, check in hbase-site.xml.
      value_to_be_replaced = GetEnv(var).value_or(GetProperty(var).value_or(""));

      // we haven't found any value yet so we are returning eval
      if (0 == value_to_be_replaced.size()) {
        return eval;
      }

      // return original expression if there is a loop
      if (value_to_be_replaced == expr) {
        return expr;
      }

      eval.replace(start_pos, var.size() + 3, value_to_be_replaced);

    } else {
      // No further expansion required.
      return eval;
    }
  }
  // We reached here if the loop is exhausted
  // If MAX_SUBSTS is exhausted, check if more variable substitution is reqd.
  // If any-more substitutions are reqd, throw an error.
  var = "";
  if (IsSubVariable(eval, var) != std::string::npos) {
    throw std::runtime_error("Variable substitution depth too large: " +
                             std::to_string(kMaxSubsts) + " " + expr);
  } else {
    return eval;
  }
}

std::optional<std::string> Configuration::GetEnv(const std::string &key) const {
  char buf[2048];

  if ("user.name" == key) {
#ifdef HAVE_GETLOGIN
    return std::make_optional(getlogin());
#else
    DLOG(WARNING) << "Client user.name not implemented";
    return std::optional<std::string>();
#endif
  }

  if ("user.dir" == key) {
#ifdef HAVE_GETCWD
    if (getcwd(buf, sizeof(buf))) {
      return std::make_optional(buf);
    } else {
      return std::optional<std::string>();
    }
#else
    DLOG(WARNING) << "Client user.dir not implemented";
    return std::optional<std::string>();
#endif
  }

  if ("user.home" == key) {
#if defined(HAVE_GETUID) && defined(HAVE_GETPWUID_R)
    uid = getuid();
    if (!getpwuid_r(uid, &pw, buf, sizeof(buf), &pwp)) {
      return std::make_optional(buf);
    } else {
      return std::optional<std::string>();
    }
#else
    DLOG(WARNING) << "Client user.home not implemented";
    return std::optional<std::string>();
#endif
  }
  return std::optional<std::string>();
}

std::optional<std::string> Configuration::GetProperty(const std::string &key) const {
  auto found = hb_property_.find(key);
  if (found != hb_property_.end()) {
    return std::make_optional(found->second.value);
  } else {
    return std::optional<std::string>();
  }
}

std::optional<std::string> Configuration::Get(const std::string &key) const {
  std::optional<std::string> raw = GetProperty(key);
  if (raw) {
    return std::make_optional(SubstituteVars(*raw));
  } else {
    return std::optional<std::string>();
  }
}

std::string Configuration::Get(const std::string &key, const std::string &default_value) const {
  return Get(key).value_or(default_value);
}

std::optional<int32_t> Configuration::GetInt(const std::string &key) const {
  std::optional<std::string> raw = Get(key);
  if (raw) {
    try {
      return std::make_optional(boost::lexical_cast<int32_t>(*raw));
    } catch (const boost::bad_lexical_cast &blex) {
      throw std::runtime_error(blex.what());
    }
  }
  return std::optional<int32_t>();
}

int32_t Configuration::GetInt(const std::string &key, int32_t default_value) const {
  return GetInt(key).value_or(default_value);
}

std::optional<int64_t> Configuration::GetLong(const std::string &key) const {
  std::optional<std::string> raw = Get(key);
  if (raw) {
    try {
      return std::make_optional(boost::lexical_cast<int64_t>(*raw));
    } catch (const boost::bad_lexical_cast &blex) {
      throw std::runtime_error(blex.what());
    }
  }
  return std::optional<int64_t>();
}

int64_t Configuration::GetLong(const std::string &key, int64_t default_value) const {
  return GetLong(key).value_or(default_value);
}

std::optional<double> Configuration::GetDouble(const std::string &key) const {
  std::optional<std::string> raw = Get(key);
  if (raw) {
    try {
      return std::make_optional(boost::lexical_cast<double>(*raw));
    } catch (const boost::bad_lexical_cast &blex) {
      throw std::runtime_error(blex.what());
    }
  }
  return std::optional<double>();
}

double Configuration::GetDouble(const std::string &key, double default_value) const {
  return GetDouble(key).value_or(default_value);
}

std::optional<bool> Configuration::GetBool(const std::string &key) const {
  std::optional<std::string> raw = Get(key);
  if (raw) {
    if (!strcasecmp((*raw).c_str(), "true") || !strcasecmp((*raw).c_str(), "1")) {
      return std::make_optional(true);
    } else if (!strcasecmp((*raw).c_str(), "false") || !strcasecmp((*raw).c_str(), "0")) {
      return std::make_optional(false);
    } else {
      boost::format what("Unexpected value \"%s\" found being converted to bool for key \"%s\"");
      what % (*raw);
      what % key;
      throw std::runtime_error(what.str());
    }
  }
  return std::optional<bool>();
}

bool Configuration::GetBool(const std::string &key, bool default_value) const {
  return GetBool(key).value_or(default_value);
}

void Configuration::Set(const std::string &key, const std::string &value) {
  hb_property_[key] = value;
}

void Configuration::SetInt(const std::string &key, int32_t value) {
  Set(key, boost::lexical_cast<std::string>(value));
}

void Configuration::SetLong(const std::string &key, int64_t value) {
  Set(key, boost::lexical_cast<std::string>(value));
}

void Configuration::SetDouble(const std::string &key, double value) {
  Set(key, boost::lexical_cast<std::string>(value));
}

void Configuration::SetBool(const std::string &key, bool value) {
  Set(key, boost::lexical_cast<std::string>(value));
}

} /* namespace hbase */
