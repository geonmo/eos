//------------------------------------------------------------------------------
// File: Utils.hh
// Author: Elvin Sindrilaru - CERN
//------------------------------------------------------------------------------

/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2024 CERN/Switzerland                                  *
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

#include "common/Utils.hh"
#include "common/Logging.hh"
#include "common/SymKeys.hh"
#include "common/StringTokenizer.hh"
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "zlib.h"

EOSCOMMONNAMESPACE_BEGIN

//------------------------------------------------------------------------------
// Open random temporary file in /tmp/
//------------------------------------------------------------------------------
std::string MakeTemporaryFile(std::string& pattern)
{
  int tmp_fd = mkstemp((char*)pattern.c_str());

  if (tmp_fd == -1) {
    eos_static_crit("%s", "msg=\"failed to create temporary file!\"");
    return "";
  }

  (void) close(tmp_fd);
  return pattern;
}

//-----------------------------------------------------------------------------
// Make sure that geotag contains only alphanumeric segments which
// are no longer than 8 characters, in <tag1>::<tag2>::...::<tagN> format.
//-----------------------------------------------------------------------------
std::string SanitizeGeoTag(const std::string& geotag)
{
  if (geotag.empty()) {
    return std::string("Error: empty geotag");
  }

  if (geotag == "<none>") {
    return geotag;
  }

  std::string tmp_tag(geotag);
  auto segments = eos::common::StringTokenizer::split<std::vector<std::string>>
                  (tmp_tag, ':');
  tmp_tag.clear();

  for (const auto& segment : segments) {
    if (segment.empty()) {
      continue;
    }

    if (segment.length() > 8) {
      return std::string("Error: geotag segment '" + segment +
                         "' is longer than 8 chars");
    }

    for (const auto& c : segment) {
      if (!std::isalnum(c)) {
        return std::string("Error: geotag segment '" + segment + "' "
                           "contains non-alphanumeric char '" + c + "'");
      }
    }

    tmp_tag += segment;
    tmp_tag += "::";
  }

  if (tmp_tag.length() <= 2) {
    return std::string("Error: empty geotag");
  }

  tmp_tag.erase(tmp_tag.length() - 2);

  if (tmp_tag != geotag) {
    return std::string("Error: invalid geotag format '" + geotag + "'");
  }

  return tmp_tag;
}

//------------------------------------------------------------------------------
// Get (keytab) file adler checksum
//------------------------------------------------------------------------------
bool
GetFileAdlerXs(std::string& adler_xs, const std::string& fn)
{
  int fd = ::open(fn.c_str(), O_RDONLY);

  if (fd >= 0) {
    char buffer[65535];
    size_t nread = ::read(fd, buffer, sizeof(buffer));
    unsigned int adler_val = adler32(0L, Z_NULL, 0);

    while (nread > 0) {
      adler_val = adler32(adler_val, (const Bytef*) buffer, nread);
      nread = ::read(fd, buffer, sizeof(buffer));
    }

    (void) close(fd);
    char result[1024];
    snprintf(result, sizeof(result) - 1, "%08x", adler_val);
    adler_xs = result;
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
// Get binary SHA1 of given (keytab) File
//------------------------------------------------------------------------------
bool
GetFileBinarySha1(std::string& bin_sha1, const std::string& fn)
{
  std::ifstream file(fn);

  if (file.is_open()) {
    uint64_t fsize = file.tellg();

    if (fsize > 1024 * 1024) {
      eos_static_err("msg=\"file too big >1MB\", path=\"%s\"", fn.c_str());
      return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    bin_sha1 = eos::common::SymKey::BinarySha1(ss.str());
    return true;
  }

  return false;
}


//------------------------------------------------------------------------------
// Get SHA-1 hex digest of given (keytab) file
//------------------------------------------------------------------------------
bool
GetFileHexSha1(std::string& hex_sha1, const std::string& fn)
{
  std::string bin_sha1;

  if (!GetFileBinarySha1(bin_sha1, fn)) {
    return false;
  }

  unsigned char* ptr = (unsigned char*)bin_sha1.data();
  unsigned int sz_result = bin_sha1.size();
  std::ostringstream oss;
  oss.fill('0');
  oss << std::hex;

  for (unsigned int i = 0; i < sz_result; ++i) {
    oss << std::setw(2) << (unsigned int) *ptr;
    ++ptr;
  }

  hex_sha1 = oss.str();
  return true;
}

EOSCOMMONNAMESPACE_END
