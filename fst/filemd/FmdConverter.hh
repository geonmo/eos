/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2022 CERN/Switzerland                           *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 ************************************************************************
 */


#ifndef EOS_FMDCONVERTER_HH
#define EOS_FMDCONVERTER_HH

#include "fst/filemd/FmdHandler.hh"
#include <folly/futures/Future.h>
#include <memory>

namespace folly {
class Executor;
} // namespace folly

namespace eos::fst {

static constexpr std::string_view ATTR_CONVERSION_DONE_FILE = ".eosattrconverted";
static constexpr size_t MIN_FMDCONVERTER_THREADS=2;
static constexpr size_t MAX_FMDCONVERTER_THREADS=100;
/*!
 * A simple interface to track whether full conversions have been done for a given
 * FST mount path. The implementation is supposed to track whether the FST is
 * done converting or mark the FST as converted when we're done converting.
 */
struct FSConversionDoneHandler {
  virtual bool isFSConverted(std::string_view fstpath) = 0;
  virtual bool markFSConverted(std::string_view fstpath) = 0;
  virtual ~FSConversionDoneHandler() = default;
};

class FmdConverter {
public:
  FmdConverter(FmdHandler * src_handler,
               FmdHandler * tgt_handler,
               size_t per_disk_pool);

  folly::Future<bool> Convert(eos::common::FileSystem::fsid_t fsid,
                              std::string_view path);
  void ConvertFS(std::string_view fspath);
  void ConvertFS(std::string_view fspath,
                 eos::common::FileSystem::fsid_t fsid);
private:
  FmdHandler * mSrcFmdHandler;
  FmdHandler * mTgtFmdHandler;
  std::unique_ptr<folly::Executor> mExecutor;
  std::unique_ptr<FSConversionDoneHandler> mDoneHandler;
};

class FileFSConversionDoneHandler final: public FSConversionDoneHandler {
public:
  FileFSConversionDoneHandler(std::string_view fname) : mConversionDoneFile(fname)
  {}
  bool isFSConverted(std::string_view fstpath) override;
  bool markFSConverted(std::string_view fstpath) override;
  std::string getDoneFilePath(std::string_view fstpath);
private:
  std::string mConversionDoneFile;
};

} // namespace eos::fst

#endif // EOS_FMDCONVERTER_HH
