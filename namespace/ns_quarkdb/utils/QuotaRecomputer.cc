/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2019 CERN/Switzerland                                  *
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
 ************************************************************************/

//------------------------------------------------------------------------------
//! @author Georgios Bitzes <georgios.bitzes@cern.ch>
//! @brief Utility class to recompute a quotanode
//------------------------------------------------------------------------------

#include "namespace/ns_quarkdb/utils/QuotaRecomputer.hh"
#include "namespace/ns_quarkdb/views/HierarchicalView.hh"
#include "namespace/Constants.hh"
#include "namespace/ns_quarkdb/explorer/NamespaceExplorer.hh"
#include "common/LayoutId.hh"

EOSNSNAMESPACE_BEGIN

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
QuotaRecomputer::QuotaRecomputer(IView *view, qclient::QClient *qcl,
  folly::Executor *exec) : mView(view), mQcl(qcl), mExecutor(exec) {}

//------------------------------------------------------------------------------
// Filtering class for NamespaceExplorer to ignore sub-quotanodes when
// recomputing a quotanode.
//------------------------------------------------------------------------------
class QuotaNodeFilter : public ExpansionDecider
{
public:
  QuotaNodeFilter(uint64_t root) : rootContainer(root) {}

  virtual bool shouldExpandContainer(const eos::ns::ContainerMdProto& proto,
                                     const eos::IContainerMD::XAttrMap& attrs) override
  {
    if(proto.id() == rootContainer) {
      return true; // always expand root, no matter what
    }

    if ((proto.flags() & eos::QUOTA_NODE_FLAG) == 0) {
      return true; // not a quota node, continue
    }

    return false; // quota node, ignore
  }

private:
  uint64_t rootContainer;
};

//------------------------------------------------------------------------------
// Given a quotanode, re-calculate the quota values,
// store into QuotaNodeCore.
//------------------------------------------------------------------------------
MDStatus QuotaRecomputer::recompute(IContainerMDPtr quotanode, QuotaNodeCore &qnc) {
  // Reset qnc contents
  qnc = {};

  if(!quotanode) {
    return MDStatus(EINVAL, "Received null quotanode!");
  }

  if ((quotanode->getFlags() & eos::QUOTA_NODE_FLAG) == 0) {
    return MDStatus(EINVAL, "Specified directory is not a quota node");
  }

  if(mView->inMemory()) {
    return MDStatus(EINVAL, "Quota recomputation is only availbale for QDB namespace");
  }

  ExplorationOptions options;
  options.depthLimit = 2048;
  options.expansionDecider.reset(new QuotaNodeFilter(quotanode->getId()));
  NamespaceExplorer explorer(mView->getUri(quotanode.get()),
                             options,
                             *mQcl,
                             mExecutor);
  NamespaceItem item;

  while (explorer.fetch(item)) {
    if (item.isFile) {
      // Calculate physical size
      uint64_t logicalSize = item.fileMd.size();
      uint64_t physicalSize = item.fileMd.size() *
                              eos::common::LayoutId::GetSizeFactor(item.fileMd.layout_id());
      // Account file.
      qnc.addFile(
        item.fileMd.uid(),
        item.fileMd.gid(),
        logicalSize,
        physicalSize
      );
    }
  }

  return MDStatus(); // OK
}

EOSNSNAMESPACE_END
