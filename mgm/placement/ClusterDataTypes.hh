// ----------------------------------------------------------------------
// File: ClusterDataTypes
// Author: Abhishek Lekshmanan - CERN
// ----------------------------------------------------------------------

/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2023 CERN/Switzerland                           *
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

#ifndef EOS_CLUSTERDATATYPES_HH
#define EOS_CLUSTERDATATYPES_HH

namespace eos::mgm::placement {

using eos::common::FileSystem::fsid_t;

// We use a item_id to represent a storage element, negative numbers represent
// storage elements in the hierarchy, ie. groups/racks/room/site etc.
using item_id_t = int32_t;
using epoch_id_t = uint64_t;
// A struct representing a disk, this is the lowest level of the hierarchy,
// disk ids map 1:1 to fsids, however it is necessary that the last bit of fsid_t
// is not used, as we use a int32_t for the rest of the placement hierarchy.
// the struct is packed to 8 bytes, so upto 8192 disks
// can fit in a single 64kB cache, it is recommended to keep this struct aligned
struct Disk {
  fsid_t id;
  mutable std::atomic<common::ConfigStatus> status {common::ConfigStatus::kUnknown};
  mutable std::atomic<uint8_t> weight{0}; // we really don't need floating point precision
  mutable std::atomic<uint8_t> percent_used{0};
  /* We've one byte left for future use - remove this line when that happens */

  explicit Disk(fsid_t _id) : id(_id) {}

  Disk(fsid_t _id, common::ConfigStatus _status, uint8_t _weight,
       uint8_t _percent_used = 0)
      : id(_id), status(_status), weight(_weight), percent_used(_percent_used)
  {
  }

  // explicit copy constructor as atomic types are not copyable
  Disk(const Disk& other)
      : Disk(other.id, other.status.load(std::memory_order_relaxed),
             other.weight.load(std::memory_order_relaxed),
             other.percent_used.load(std::memory_order_relaxed))
  {
  }

  friend bool
  operator<(const Disk& l, const Disk& r)
  {
    return l.id < r.id;
  }
};

static_assert(sizeof(Disk) == 8, "Disk data type not aligned to 8 bytes!");

// some common storage elements, these could be user defined in the future
enum class StdBucketType : uint8_t {
  GROUP,
  RACK,
  ROOM,
  SITE,
  ROOT };

constexpr uint8_t
get_bucket_type(StdBucketType t)
{
  return static_cast<uint8_t>(t);
}

struct Bucket {
  item_id_t id;
  uint32_t total_weight;
  uint8_t bucket_type;
  std::vector<item_id_t> items;

  Bucket(item_id_t _id, uint8_t type)
      : id(_id), total_weight(0), bucket_type(type)
  {
  }

  friend bool
  operator<(const Bucket& l, const Bucket& r)
  {
    return l.id < r.id;
  }
};

struct ClusterData {
  std::vector<Disk> disks;
  std::vector<Bucket> buckets;
};

inline bool isValidBucketId(item_id_t id, const ClusterData& data) {
  return id < 0 && (-id < data.buckets.size());
}

} // eos::mgm::placement

#endif // EOS_CLUSTERDATATYPES_HH
