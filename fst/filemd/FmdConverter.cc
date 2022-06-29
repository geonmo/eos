#include "fst/filemd/FmdConverter.hh"
#include "fst/utils/StdFSWalkTree.hh"

#include "common/Logging.hh"
#include "fst/utils/FSPathHandler.hh"
#include "fst/utils/StdFSWalkTree.hh"
#include <folly/executors/IOThreadPoolExecutor.h>

namespace eos::fst
{


FmdConverter::FmdConverter(FmdHandler* src_handler,
                           FmdHandler* tgt_handler,
                           size_t per_disk_pool) :
  mSrcFmdHandler(src_handler), mTgtFmdHandler(tgt_handler),
  mExecutor(std::make_unique<folly::IOThreadPoolExecutor>(per_disk_pool))
{}


folly::Future<bool>
FmdConverter::Convert(eos::common::FileSystem::fsid_t fsid,
                      std::string_view path)
{
  auto fid = eos::common::FileId::PathToFid(path.data());

  if (!fsid || !fid) {
    return false;
  }

  return mTgtFmdHandler->ConvertFrom(fid, fsid, mSrcFmdHandler, true);
}

void
FmdConverter::ConvertFS(std::string_view fspath)
{
  std::error_code ec;
  auto fsid = FSPathHandler::GetFsid(fspath);
  stdfs::WalkFSTree(fspath, [this, fsid](std::string path) {
    this->Convert(fsid, path)
    .via(mExecutor.get())
    .thenValue([path = std::move(path)](bool status) {
      eos_static_info("msg=\"Conversion status\" file=%s, status=%d",
                      path.c_str(),
                      status);
    });
  }, ec);
}

} // namespace eos::fst
