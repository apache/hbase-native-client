##
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This Dockerfile is shared for both personal use as well as precommit testing. Certain changes might break Yetus
# precommit test-patch integration, so be mindful of that when making changes.

FROM ubuntu:16.04

ARG CC=/usr/bin/gcc-5
ARG CXX=/usr/bin/g++-5
ARG CFLAGS="-fPIC -g -fno-omit-frame-pointer -O2 -pthread"
ARG CXXFLAGS="-fPIC -g -fno-omit-frame-pointer -O2 -pthread"

ENV JAVA_HOME="/usr/lib/jvm/java-8-openjdk-amd64/"

RUN apt-get update && \
    apt-get install -y vim maven inetutils-ping python-pip doxygen graphviz clang-format valgrind \
        wget libgflags-dev libgoogle-glog-dev dh-autoreconf pkg-config libssl-dev build-essential \
        libevent-dev cmake libkrb5-dev git openjdk-8-jdk curl unzip google-mock libsodium-dev && \
    pip install yapf && \
    apt-get -qq clean && \
    apt-get -y -qq autoremove && \
    rm -rf /var/lib/{apt,dpkg,cache,log}/ && \
    rm -rf /tmp/*

## Install Google Test to support 
RUN wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz && \
  	tar zxf release-1.8.0.tar.gz && \
  	rm -f release-1.8.0.tar.gz && \
  	mv googletest-release-1.8.0 gtest && \
  	cd gtest && \
  	cmake . && \
  	make -j4 && \
 	make install

RUN apt-get update && \
    apt-get install -y debconf-utils && \
    echo "krb5-config krb5-config/kerberos_servers string localhost" | debconf-set-selections ; \
    echo "krb5-config krb5-config/admin_server string localhost" | debconf-set-selections ; \
    echo "krb5-config krb5-config/add_servers_realm string EXAMPLE.COM" | debconf-set-selections ; \
    echo "krb5-config krb5-config/default_realm string EXAMPLE.COM" | debconf-set-selections ; \
    apt-get install -y krb5-kdc krb5-admin-server ; \
    echo "admin" > /tmp/krb-realm.pass ; \
    echo "admin" >> /tmp/krb-realm.pass ; \
    krb5_newrealm < /tmp/krb-realm.pass ; \
    echo "addprinc hbase" > /tmp/krb-princ.pass ; \
    echo "admin" >> /tmp/krb-princ.pass ; \
    echo "admin" >> /tmp/krb-princ.pass ; \
    kadmin.local < /tmp/krb-princ.pass ; \
    echo 'addprinc hbase/securecluster' > /tmp/krb-princ.pass; echo 'admin' >> /tmp/krb-princ.pass ; \
    rm -f hbase-host.keytab ; echo 'admin' >> /tmp/krb-princ.pass ; \
    echo 'xst -k hbase-host.keytab hbase/securecluster@EXAMPLE.COM' >> /tmp/krb-princ.pass ; \
    kadmin.local < /tmp/krb-princ.pass ;
 
COPY krb5.conf /etc

RUN echo "enabled=1" >> /etc/default/apport

WORKDIR /usr/src/hbase/hbase-native-client

CMD ["/usr/sbin/krb5kdc -P /var/run/krb5kdc.pid && sysctl -p && ulimit -c unlimited && /bin/bash"]

###
# Everything past this point is either not needed for testing or breaks Yetus.
# So tell Yetus not to read the rest of the file:
# YETUS CUT HERE
###

