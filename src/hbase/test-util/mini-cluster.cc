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

#include "hbase/test-util/mini-cluster.h"
#include <fcntl.h>
#include <glog/logging.h>
#include <fstream>

using hbase::MiniCluster;
using std::ifstream;
using std::lock_guard;
using std::mutex;
using std::string;
using std::vector;

JNIEnv *MiniCluster::CreateVM(JavaVM **jvm) {
  JavaVMInitArgs args;
  JavaVMOption jvm_options;
  args.version = JNI_VERSION_1_6;
  args.nOptions = 1;
  // Sets the Java classpath to load the test jars and dependencies.
  // Either set the CLASSPATH environment variable before running the test or the test loads it from the default
  // location, if it exists.
  char *classpath = getenv("CLASSPATH");
  // Copy it to a string so that we don't inadverdently change the environment variable.
  string final_classpath;
  if (classpath == nullptr || strstr(classpath, "-tests.jar") == nullptr) {
    if (classpath != nullptr) {
      final_classpath.assign(classpath);
    }
    // Default classpath loaded from downloaded HBase src (paths defined in CMakeLists.txt)
    string clspath_file_path("./apachehbase-src/hbase-build-configuration/target/cached_classpath.txt");
    ifstream fd(clspath_file_path);
    if (!fd.is_open()) {
      LOG(FATAL) << "No valid classpath found. If you haven't built with DOWNLOAD_DEPENDENCIES=ON, set the appropriate"
                    "CLASSPATH variable pointing to the content of cached_classpath.txt generated from compiling HBase"
                    "sources.";
    }
    string file_contents;
    getline(fd, file_contents);
    if (file_contents.empty()) {
      LOG(FATAL) << "Empty classpath file encountered: " << clspath_file_path;
    }
    final_classpath.append(":" + file_contents);
  }
  auto options = string{"-Djava.class.path="} + final_classpath;
  jvm_options.optionString = const_cast<char *>(options.c_str());
  args.options = &jvm_options;
  args.ignoreUnrecognized = 0;
  int rv = JNI_CreateJavaVM(jvm, reinterpret_cast<void **>(&env_), &args);
  CHECK_NOTNULL(env_);
  CHECK(rv >= 0);
  return env_;
}

MiniCluster::~MiniCluster() {
  if (!env_) {
    return;
  }
  StopCluster();
  // Clean up the global references.
  if (testing_util_class_) {
    env_->DeleteGlobalRef(testing_util_class_);
  }
  if (table_name_class_) {
    env_->DeleteGlobalRef(table_name_class_);
  }
  if (conf_class_) {
    env_->DeleteGlobalRef(conf_class_);
  }
  if (put_class_) {
    env_->DeleteGlobalRef(put_class_);
  }
  if (htu_) {
    env_->DeleteGlobalRef(htu_);
  }
  if (jvm_ != nullptr) {
    jvm_->DestroyJavaVM();
    jvm_ = nullptr;
  }
}

void MiniCluster::Setup() {
  lock_guard<mutex> lock(lock_);
  if (inited_) return;
  if (!env_) {
    env_ = CreateVM(&jvm_);
    CHECK_NOTNULL(env_);
  }
  testing_util_class_ = FindClassAndGetGlobalRef("org/apache/hadoop/hbase/HBaseTestingUtility");
  jmethodID mid = env_->GetMethodID(testing_util_class_, "<init>","()V");
  CHECK_NOTNULL(mid);
  jobject  local_htu = env_->NewObject(testing_util_class_, mid);
  if (!local_htu) {
    LOG(FATAL) << "Couldn't invoke method createLocalHTU in HBaseTestingUtility";
  }
  htu_ = env_->NewGlobalRef(local_htu);
  env_->DeleteLocalRef(local_htu);
  get_conn_mid_ = env_->GetMethodID(testing_util_class_, "getConnection",
      "()Lorg/apache/hadoop/hbase/client/Connection;");
  jclass conn_class = env_->FindClass("org/apache/hadoop/hbase/client/Connection");
  get_admin_mid_ =
      env_->GetMethodID(conn_class, "getAdmin", "()Lorg/apache/hadoop/hbase/client/Admin;");
  get_table_mid_ = env_->GetMethodID(conn_class, "getTable",
      "(Lorg/apache/hadoop/hbase/TableName;)Lorg/apache/hadoop/hbase/client/Table;");
  if (!get_table_mid_) {
    LOG(FATAL) << "Couldn't find getConnection";
  }
  jclass adminClass = env_->FindClass("org/apache/hadoop/hbase/client/Admin");
  move_mid_ = env_->GetMethodID(adminClass, "move", "([B[B)V");
  if (move_mid_ == nullptr) {
    LOG(FATAL) << "Couldn't find move";
  }
  create_table_mid_ = env_->GetMethodID(testing_util_class_, "createTable",
      "(Lorg/apache/hadoop/hbase/TableName;Ljava/lang/String;)Lorg/apache/hadoop/hbase/client/Table;");
  create_table_families_mid_ = env_->GetMethodID(testing_util_class_, "createTable",
      "(Lorg/apache/hadoop/hbase/TableName;[[B)Lorg/apache/hadoop/hbase/client/Table;");
  create_table_with_split_mid_ = env_->GetMethodID(testing_util_class_, "createTable",
      "(Lorg/apache/hadoop/hbase/TableName;[[B[[B)Lorg/apache/hadoop/hbase/client/Table;");
  if (create_table_with_split_mid_ == nullptr) {
    LOG(FATAL) << "Couldn't find method createTable with split";
  }
  table_name_class_ = FindClassAndGetGlobalRef("org/apache/hadoop/hbase/TableName");
  tbl_name_value_of_mid_ = env_->GetStaticMethodID(
      table_name_class_, "valueOf", "(Ljava/lang/String;)Lorg/apache/hadoop/hbase/TableName;");
  if (tbl_name_value_of_mid_ == nullptr) {
    LOG(FATAL) << "Couldn't find method valueOf in TableName";
  }
  jclass hbaseMiniClusterClass = env_->FindClass("org/apache/hadoop/hbase/MiniHBaseCluster");
  stop_rs_mid_ =env_->GetMethodID(hbaseMiniClusterClass, "stopRegionServer",
      "(I)Lorg/apache/hadoop/hbase/util/JVMClusterUtil$RegionServerThread;");
  get_conf_mid_ = env_->GetMethodID(hbaseMiniClusterClass, "getConfiguration",
      "()Lorg/apache/hadoop/conf/Configuration;");
  conf_class_ = FindClassAndGetGlobalRef("org/apache/hadoop/conf/Configuration");
  set_conf_mid_ = env_->GetMethodID(conf_class_, "set", "(Ljava/lang/String;Ljava/lang/String;)V");
  if (set_conf_mid_ == nullptr) {
    LOG(FATAL) << "Couldn't find method getConf in MiniHBaseCluster";
  }
  conf_get_mid_ = env_->GetMethodID(conf_class_, "get", "(Ljava/lang/String;)Ljava/lang/String;");
  jclass tableClass = env_->FindClass("org/apache/hadoop/hbase/client/Table");
  put_mid_ = env_->GetMethodID(tableClass, "put", "(Lorg/apache/hadoop/hbase/client/Put;)V");
  jclass connFactoryClass = env_->FindClass("org/apache/hadoop/hbase/client/ConnectionFactory");
  create_conn_mid_ = env_->GetStaticMethodID(connFactoryClass, "createConnection",
      "()Lorg/apache/hadoop/hbase/client/Connection;");
  if (create_conn_mid_ == nullptr) {
    LOG(FATAL) << "Couldn't find createConnection";
  }
  put_class_ = FindClassAndGetGlobalRef("org/apache/hadoop/hbase/client/Put");
  put_ctor_ = env_->GetMethodID(put_class_, "<init>", "([B)V");
  add_col_mid_ = env_->GetMethodID(put_class_, "addColumn", "([B[B[B)Lorg/apache/hadoop/hbase/client/Put;");
  if (add_col_mid_ == nullptr) {
    LOG(FATAL) << "Couldn't find method addColumn";
  }
  inited_ = true;
}

// converts C char* to Java byte[]
jbyteArray MiniCluster::StrToByteChar(const string &str) {
  if (str.length() == 0) {
    return nullptr;
  }
  int n = str.length();
  jbyteArray arr = env_->NewByteArray(n);
  env_->SetByteArrayRegion(arr, 0, n, reinterpret_cast<const jbyte *>(str.c_str()));
  return arr;
}

jobject MiniCluster::CreateTable(const string &table, const string &family) {
  jstring table_name_str = env_->NewStringUTF(table.c_str());
  jobject table_name =
      env_->CallStaticObjectMethod(table_name_class_, tbl_name_value_of_mid_, table_name_str);
  jstring family_str = env_->NewStringUTF(family.c_str());
  jobject table_obj = env_->CallObjectMethod(htu_, create_table_mid_, table_name, family_str);
  return table_obj;
}

jobject MiniCluster::CreateTable(const string &table, const vector<string> &families) {
  jstring table_name_str = env_->NewStringUTF(table.c_str());
  jobject table_name =
      env_->CallStaticObjectMethod(table_name_class_, tbl_name_value_of_mid_, table_name_str);
  jclass array_element_type = env_->FindClass("[B");
  jobjectArray family_array = env_->NewObjectArray(families.size(), array_element_type, nullptr);
  int i = 0;
  for (const auto& family : families) {
    env_->SetObjectArrayElement(family_array, i++, StrToByteChar(family));
  }
  jobject table_obj =
      env_->CallObjectMethod(htu_, create_table_families_mid_, table_name, family_array);
  return table_obj;
}

jobject MiniCluster::CreateTable(const string &table, const string &family,
                                 const vector<string> &keys) {
  vector<string> families{};
  families.emplace_back(family);
  return CreateTable(table, families, keys);
}

jobject MiniCluster::CreateTable(const string &table, const vector<string> &families,
                                 const vector<string> &keys) {
  jstring table_name_str = env_->NewStringUTF(table.c_str());
  jobject table_name =
      env_->CallStaticObjectMethod(table_name_class_, tbl_name_value_of_mid_, table_name_str);
  jclass array_element_type = env_->FindClass("[B");

  int i = 0;
  jobjectArray family_array = env_->NewObjectArray(families.size(), array_element_type, nullptr);
  for (const auto& family : families) {
    env_->SetObjectArrayElement(family_array, i++, StrToByteChar(family));
  }

  jobjectArray key_array = env_->NewObjectArray(keys.size(), array_element_type, nullptr);

  i = 0;
  for (const auto& key : keys) {
    env_->SetObjectArrayElement(key_array, i++, StrToByteChar(key));
  }

  jobject tbl = env_->CallObjectMethod(htu_, create_table_with_split_mid_, table_name, family_array,
                                       key_array);
  return tbl;
}

// returns the Configuration for the cluster
jobject MiniCluster::GetConf() {
  CHECK_NOTNULL(env_);
  return env_->CallObjectMethod(cluster_, get_conf_mid_);
}
// return the Admin instance for the local cluster
jobject MiniCluster::admin() {
  CHECK_NOTNULL(env_);
  jobject conn = env_->CallObjectMethod(htu_, get_conn_mid_);
  jobject admin = env_->CallObjectMethod(conn, get_admin_mid_);
  return admin;
}

// moves region to server
void MiniCluster::MoveRegion(const string &region, const string &server) {
  CHECK_NOTNULL(env_);
  jobject admin_ = admin();
  env_->CallObjectMethod(admin_, move_mid_, StrToByteChar(region), StrToByteChar(server));
}

jobject MiniCluster::StartCluster(int num_region_servers) {
  Setup();
  CHECK_NOTNULL(env_);
  jmethodID mid = env_->GetMethodID(testing_util_class_, "startMiniCluster",
      "(I)Lorg/apache/hadoop/hbase/MiniHBaseCluster;");
  if (mid == nullptr) {
    LOG(INFO) << "Couldn't find method startMiniCluster in the class HBaseTestingUtility";
    exit(-1);
  }
  cluster_ = env_->CallObjectMethod(htu_, mid, static_cast<jint>(num_region_servers));
  return cluster_;
}

string MiniCluster::GetConfValue(const string &key) {
  jobject conf = GetConf();
  auto jval =
      (jstring)env_->CallObjectMethod(conf, conf_get_mid_, env_->NewStringUTF(key.c_str()));
  const char *val = env_->GetStringUTFChars(jval, 0);
  return val;
}

jclass hbase::MiniCluster::FindClassAndGetGlobalRef(const char *cls_name) {
  // Make sure env is initialized.
  CHECK_NOTNULL(env_);
  jclass local_ref = env_->FindClass(cls_name);
  if (!local_ref) {
    LOG(FATAL) << "Couldn't find class: " << cls_name;
  }
  jobject global_ref = env_->NewGlobalRef(local_ref);
  env_->DeleteLocalRef(local_ref);
  return static_cast<jclass>(global_ref);
}

void hbase::MiniCluster::StopCluster() {
  CHECK_NOTNULL(env_);
  lock_guard<mutex> lock(lock_);
  if (testing_util_class_ && htu_) {
    LOG(INFO) << "Shutting down mini cluster.";
    // Clean up the test cluster.
    jmethodID mid = env_->GetMethodID(testing_util_class_, "shutdownMiniCluster", "()V");
    env_->CallVoidMethod(htu_, mid);
  }
}
