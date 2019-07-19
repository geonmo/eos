
/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2017 CERN/Switzerland                                  *
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

#pragma once
#include "common/Namespace.hh"
#include "proto/FmdBase.pb.h"
#include "common/FileSystem.hh"
#include "common/Logging.hh"
#include "common/FileId.hh"

EOSCOMMONNAMESPACE_BEGIN

//------------------------------------------------------------------------------
//! Class modelling the file metadata stored on the FST. It wrapps an FmdBase
//! class generated from the ProtoBuffer specification and adds some extra
//! functionality for conversion to/from string/env representation.
//------------------------------------------------------------------------------
class FmdHelper : public eos::common::LogId
{
public:
  static constexpr uint64_t UNDEF = 0xfffffffffff1ULL;

  //---------------------------------------------------------------------------
  //! Constructor
  //---------------------------------------------------------------------------
  FmdHelper(eos::common::FileId::fileid_t fid = 0, int fsid = 0)
  {
    Reset();
    mProtoFmd.set_fid(fid);
    mProtoFmd.set_fsid(fsid);
  }

  //---------------------------------------------------------------------------
  //! Destructor
  //---------------------------------------------------------------------------
  virtual ~FmdHelper() = default;

  //---------------------------------------------------------------------------
  //! Compute layout error
  //!
  //! @param fsid file system id to check against
  //!
  //! @return 0 if there are no errors, otherwise encoded type of layout error
  //!        stored in the int.
  //---------------------------------------------------------------------------
  int LayoutError(eos::common::FileSystem::fsid_t fsid);

  //---------------------------------------------------------------------------
  //! Reset file meta data object
  //!
  //! @param fmd protobuf file meta data
  //---------------------------------------------------------------------------
  void Reset();

  //---------------------------------------------------------------------------
  //! Get set of locations for the given fmd
  //!
  //! @param valid_replicas number of valid replicas <= size of the returned
  //!        set i.e. replicas which are not unlinked
  //!
  //! @return set of file system ids representing the locations
  //---------------------------------------------------------------------------
  std::set<eos::common::FileSystem::fsid_t>
  GetLocations(size_t& valid_replicas);

  //---------------------------------------------------------------------------
  //! Convert fmd object to env representation
  //!
  //! @return XrdOucEnv holding information about current object
  //---------------------------------------------------------------------------
  std::unique_ptr<XrdOucEnv> FmdToEnv();

  //---------------------------------------------------------------------------
  //! File meta data object replication function (copy constructor)
  //---------------------------------------------------------------------------
  void
  Replicate(FmdHelper& fmd)
  {
    mProtoFmd = fmd.mProtoFmd;
  }

  eos::fst::FmdBase mProtoFmd; ///< Protobuf file metadata info
};

//------------------------------------------------------------------------------
//! Convert an FST env representation to an Fmd struct
//!
//! @param env env representation
//! @param fmd reference to Fmd struct
//!
//! @return true if successful, otherwise false
//------------------------------------------------------------------------------
bool EnvToFstFmd(XrdOucEnv& env, FmdHelper& fmd);

EOSCOMMONNAMESPACE_END
