//------------------------------------------------------------------------------
// File: JailedPath.hh
// Author: Georgios Bitzes - CERN
//------------------------------------------------------------------------------

/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2011 CERN/Switzerland                                  *
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

#ifndef FUSEX_JAILED_PATH_HH
#define FUSEX_JAILED_PATH_HH

#include <string>
#include <sys/types.h>
#include "Utils.hh"

//------------------------------------------------------------------------------
//! A type which represents a "chrooted" path: You can resolve this path
//! perfectly by chrooting into "jail" and then "stat path".
//!
//! We can't chroot: Too messy and expensive. We'll try to emulate
//! the result. Support in Linux kernel is upcoming, through openat and
//! AT_THIS_ROOT.
//!
//! We'll probably just ban symbolic links here for the moment.
//! TODO(gbitzes)
//------------------------------------------------------------------------------
class JailedPath {
public:
  //----------------------------------------------------------------------------
  //! Constructor
  //----------------------------------------------------------------------------
  JailedPath(const std::string &jail_, const std::string &path_)
  : jail(jail_), path(path_) {}

  //----------------------------------------------------------------------------
  //! Get raw path
  //----------------------------------------------------------------------------
  std::string getRawPath() const {
    // TODO(gbitzes): Fix
    return path;
  }

  //----------------------------------------------------------------------------
  //! Describe
  //----------------------------------------------------------------------------
  std::string describe() const {
    return SSTR("{ jail: " << jail << ", path: " << path << " }");
  }

  //----------------------------------------------------------------------------
  //! Check if this path is empty
  //----------------------------------------------------------------------------
  bool empty() const {
    return path.empty();
  }

  //----------------------------------------------------------------------------
  //! operator< for storing these objects in maps, etc
  //----------------------------------------------------------------------------
  bool operator<(const JailedPath& other) const {
    if(jail != other.jail) {
      return jail < other.jail;
    }

    return path < other.path;
  }

private:
  std::string jail;
  std::string path;
};


#endif