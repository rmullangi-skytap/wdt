/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "DirectorySourceQueue.h"
#include "ErrorCodes.h"
#include "Throttler.h"
#include "ClientSocket.h"

#include <chrono>
#include <memory>

DECLARE_int32(port);
DECLARE_int32(num_sockets);
DECLARE_bool(follow_symlinks);

namespace facebook {
namespace wdt {

class DirectorySourceQueue;

typedef std::chrono::high_resolution_clock Clock;

class Sender {
 public:
  Sender(const std::string &destHost, const std::string &srcDir);

  virtual ~Sender() {
  }

  std::vector<ErrorCode> start();

  void setIncludeRegex(const std::string &includeRegex);

  void setExcludeRegex(const std::string &excludeRegex);

  void setPruneDirRegex(const std::string &pruneDirRegex);

  void setPort(const int port);

  void setNumSockets(const int numSockets);

  void setSrcFileInfo(const std::vector<FileInfo> &srcFileInfo);

  void setFollowSymlinks(const bool followSymlinks);

  struct SendStats {
    size_t totalBytes = 0;
    size_t headerBytes = 0;
    size_t dataBytes = 0;
    ErrorCode errCode;
  };

  // Making the following 2 functions public for unit testing. Need to find way
  // to unit test private functions
  virtual SendStats sendOneByteSource(
      const std::unique_ptr<ClientSocket> &socket,
      const std::unique_ptr<Throttler> &throttler,
      const std::unique_ptr<ByteSource> &source, const bool doThrottling,
      const size_t totalBytes);

  void sendOne(Clock::time_point startTime, const std::string &destHost,
               int port, DirectorySourceQueue &queue, double avgRateBytes,
               double maxRateBytes, double bucketLimitBytes, SendStats &stat);

  virtual std::unique_ptr<ClientSocket> makeSocket(const std::string &destHost,
                                                   int port);

 private:
  std::string destHost_;
  int port_ = FLAGS_port;
  int numSockets_ = FLAGS_num_sockets;
  std::string srcDir_;
  std::string pruneDirRegex_;
  std::string includeRegex_;
  std::string excludeRegex_;
  std::vector<FileInfo> srcFileInfo_;
  bool followSymlinks_ = FLAGS_follow_symlinks;
};
}
}  // namespace facebook::wdt
