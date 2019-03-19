//------------------------------------------------------------------------------
// File: FsViewsTests.cc
// Author: Elvin-Alin Sindrilaru <esindril at cern dot ch>
//------------------------------------------------------------------------------

/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2018 CERN/Switzerland                                  *
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

#include "gtest/gtest.h"
#include "mgm/FsView.hh"
#include "mgm/utils/FilesystemUuidMapper.hh"

//------------------------------------------------------------------------------
// Test const_iterator implementation
//------------------------------------------------------------------------------
TEST(FsView, ConstIteratorTest)
{
  using namespace eos::mgm;
  GeoTree geo_tree;

  for (int i = 0; i < 100; ++i) {
    ASSERT_TRUE(geo_tree.insert(i));
  }

  for (int i = 0; i < 10; ++i) {
    ASSERT_FALSE(geo_tree.insert(i));
  }

  for (auto iter = geo_tree.begin(); iter != geo_tree.end(); ++iter) {
    ASSERT_TRUE(*iter >= 0);
    ASSERT_TRUE(*iter < 100);
  }

  auto iter = geo_tree.begin();
  --iter;
  ASSERT_TRUE(iter == geo_tree.begin());

  while (iter != geo_tree.end()) {
    ++iter;
  }

  ++iter;
  ASSERT_TRUE(iter == geo_tree.end());
}

//------------------------------------------------------------------------------
// Test FilesystemUuidMapper
//------------------------------------------------------------------------------
TEST(FilesystemUuidMapper, BasicSanity) {
  eos::mgm::FilesystemUuidMapper mapper;

  ASSERT_FALSE(mapper.injectMapping(0, "test"));
  ASSERT_EQ(mapper.size(), 0u);

  ASSERT_FALSE(mapper.injectMapping(0, "aaa"));
  ASSERT_EQ(mapper.size(), 0u);

  ASSERT_FALSE(mapper.injectMapping(1, ""));
  ASSERT_EQ(mapper.size(), 0u);

  ASSERT_TRUE(mapper.injectMapping(1, "fs-1"));
  ASSERT_EQ(mapper.size(), 1u);

  // conflict with fsid "1"
  ASSERT_FALSE(mapper.injectMapping(1, "fs-2"));
  ASSERT_EQ(mapper.size(), 1u);

  // conflict with uuid "fs-1"
  ASSERT_FALSE(mapper.injectMapping(2, "fs-1"));
  ASSERT_EQ(mapper.size(), 1u);

  // conflict with itself, fine, nothing changes
  ASSERT_TRUE(mapper.injectMapping(1, "fs-1"));
  ASSERT_EQ(mapper.size(), 1u);


}

