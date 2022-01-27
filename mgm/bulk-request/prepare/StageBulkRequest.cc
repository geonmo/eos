//------------------------------------------------------------------------------
//! @file StageBulkRequest.cc
//! @author Cedric Caffy - CERN
//------------------------------------------------------------------------------

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

#include "StageBulkRequest.hh"
#include "common/Logging.hh"
EOSBULKNAMESPACE_BEGIN

StageBulkRequest::StageBulkRequest(const std::string& id): BulkRequest(id)
{
}

void StageBulkRequest::addFile(std::unique_ptr<File>&& file) {
  //If there is an error, the state of the file will
  //be ERROR, even if there was a state already set
  if(file->getError()) {
    file->setState(File::State::ERROR);
  } else {
    if(!file->getState()) {
      //If no state is set to the file, the state SUBMITTED
      //is the default one
      file->setState(File::State::SUBMITTED);
    }
  }
  BulkRequest::addFile(std::move(file));
}

const BulkRequest::Type StageBulkRequest::getType() const
{
  return BulkRequest::Type::PREPARE_STAGE;
}

EOSBULKNAMESPACE_END
