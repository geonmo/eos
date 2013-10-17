/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2013 CERN/Switzerland                                  *
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
//! @file dsi_xrootd.hh
//! @author Geoffray Adde - CERN
//! @brief Interface of the XRootD DSI plugin
//------------------------------------------------------------------------------

#if !defined(DSI_XRD_HH)
#define DSI_XRD_HH

#include "XrdFileIo.hh"

extern "C" {
#include "globus_gridftp_server.h"

typedef struct globus_l_gfs_xrood_handle_s {
  // !!! WARNING
  // globus_mutex is not adapted to the XRootd async framework
  // because if threading is disbaled in the globus build
  // then, the thread model doesn't lock the mutexes because it's semantically equivalent in that case
  // but the XRootD client is multithreaded anyway
  bool isInit;
  pthread_mutex_t mutex;
  XrdFileIo *fileIo;
  globus_result_t cached_res;
  int optimal_count;
  globus_bool_t done;
  globus_off_t blk_length;
  globus_off_t blk_offset;
  globus_size_t block_size;
  globus_gfs_operation_t op;
  /* if use_uuid is true we will use uuid and fullDestPath in the
       file accessing commands */
  globus_bool_t use_uuid;

  globus_l_gfs_xrood_handle_s () : isInit(false)
  {}

} globus_l_gfs_xrootd_handle_t;

static void globus_l_gfs_file_net_read_cb
(globus_gfs_operation_t,
    globus_result_t,
    globus_byte_t *,
    globus_size_t,
    globus_off_t,
    globus_bool_t,
    void *);

static void
globus_l_gfs_xrootd_read_from_net(globus_l_gfs_xrootd_handle_t *);
static globus_result_t globus_l_gfs_make_error(const char*, int);

void fill_stat_array(globus_gfs_stat_t *, struct stat, char *);
void free_stat_array(globus_gfs_stat_t * ,int);

int next_read_chunk(globus_l_gfs_xrood_handle_s *xrootd_handle, int64_t &nextreadl);

int xrootd_handle_open(char *, int, int,
    globus_l_gfs_xrootd_handle_t *);
static globus_bool_t globus_l_gfs_xrootd_send_next_to_client
(globus_l_gfs_xrootd_handle_t *);
static void globus_l_gfs_net_write_cb(globus_gfs_operation_t,
    globus_result_t,
    globus_byte_t *,
    globus_size_t,
    void *);
} // end extern "C"

#endif  /* DSI_XRD_HH */
