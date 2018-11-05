// ----------------------------------------------------------------------
// File: TapeAwareGc.hh
// Author: Steven Murray - CERN
// ----------------------------------------------------------------------

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

#ifndef __EOSMGM_TAPEAWAREGC_HH__
#define __EOSMGM_TAPEAWAREGC_HH__

#include "common/Logging.hh"
#include "mgm/Namespace.hh"
#include "mgm/TapeAwareGcLru.hh"
#include "namespace/interface/Identifiers.hh"
#include "namespace/interface/IFileMD.hh"
#include "proto/ConsoleRequest.pb.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <stdint.h>
#include <thread>

/*----------------------------------------------------------------------------*/
/**
 * @file TapeAwareGc.hh
 *
 * @brief Class implementing a tape aware garbage collector
 *
 */
/*----------------------------------------------------------------------------*/
EOSMGMNAMESPACE_BEGIN

//------------------------------------------------------------------------------
//! Singleton class implementing a tape aware garbage collector
//------------------------------------------------------------------------------
class TapeAwareGc
{
public:

  //----------------------------------------------------------------------------
  //! Destructor
  //----------------------------------------------------------------------------
  ~TapeAwareGc();

  //----------------------------------------------------------------------------
  //! Delete copy constructor
  //----------------------------------------------------------------------------
  TapeAwareGc(const TapeAwareGc &) = delete;

  //----------------------------------------------------------------------------
  //! Delete assignment operator
  //----------------------------------------------------------------------------
  TapeAwareGc &operator=(const TapeAwareGc &) = delete;

  //----------------------------------------------------------------------------
  //! @return the single instance of this class
  //----------------------------------------------------------------------------
  static TapeAwareGc &instance();

  //----------------------------------------------------------------------------
  //! Enable the GC
  //!
  //! @param defaultSpaceMinFreeBytes Minium number of free bytes the default
  //! space should have
  //----------------------------------------------------------------------------
  void enable(const uint64_t defaultSpaceMinFreeBytes) noexcept;

  //----------------------------------------------------------------------------
  //! Notify GC the specified file has been opened
  //! @note This method does nothing and returns immediately if the GC has not
  //! been enabled
  //!
  //! @param path file path
  //! @param fmd file metadata
  //----------------------------------------------------------------------------
  void fileOpened(const std::string &path, const IFileMD &fmd) noexcept;

  //----------------------------------------------------------------------------
  //! Notify GC a replica of the specified file has been committed
  //! @note This method does nothing and returns immediately if the GC has not
  //! been enabled
  //!
  //! @param path file path
  //! @param fmd file metadata
  //----------------------------------------------------------------------------
  void fileReplicaCommitted(const std::string &path, const IFileMD &fmd)
    noexcept;

private:

  //----------------------------------------------------------------------------
  //! Boolean flag that starts with a value of false and can have timed waits on
  //! its value becoming true.
  //----------------------------------------------------------------------------
  class BlockingFlag {
  public:

    //--------------------------------------------------------------------------
    //! Constructor
    //--------------------------------------------------------------------------
    BlockingFlag(): m_flag(false) {
    }

    //--------------------------------------------------------------------------
    //! Boolean operator
    //--------------------------------------------------------------------------
    operator bool() const {
      std::unique_lock<std::mutex> lock(m_mutex);
      return m_flag;
    }

    //--------------------------------------------------------------------------
    //! Waits the specified duration for the flag to become true
    //!
    //! @param duration The amount of time to wait
    //! @return True if the flag has been set to true, else false if a timeout
    //! has occurred
    //--------------------------------------------------------------------------
    template<class Duration> bool waitForTrue(Duration duration) noexcept {
      try {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_cond.wait_for(lock, duration, [&]{return m_flag;});
      } catch(std::exception &ex) {
        eos_static_err("msg=\"%s\"", ex.what());
      } catch(...) {
        eos_static_err("msg=\"Caught an unknown exception\"");
      }
      return false;
    }

    //--------------------------------------------------------------------------
    //! Sets the flag to true and wakes all threads waiting on waitForTrue()
    //--------------------------------------------------------------------------
    void setToTrue() {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_flag = true;
      m_cond.notify_all();
    }

  private:

    //--------------------------------------------------------------------------
    //! Mutex protecting the flag
    //--------------------------------------------------------------------------
    mutable std::mutex m_mutex;

    //--------------------------------------------------------------------------
    //! The condition variable of the flag
    //--------------------------------------------------------------------------
    std::condition_variable m_cond;

    //--------------------------------------------------------------------------
    //! The flag
    //--------------------------------------------------------------------------
    bool m_flag;
  }; // class BlockingFlag


  /// Used to ensure the enable() method only starts the worker thread once
  std::atomic_flag m_enabledMethodCalled = ATOMIC_FLAG_INIT;

  /// True if the GC has been enabled
  std::atomic<bool> m_enabled;

  /// True if the worker thread should stop
  BlockingFlag m_stop;

  /// The one and only GC worker thread
  std::unique_ptr<std::thread> m_worker;

  /// Minimum number of free bytes the default space should have
  uint64_t m_defaultSpaceMinFreeBytes;

  /// Mutex protecting mLruQueue
  std::mutex m_lruQueueMutex;

  /// Queue of Least Recently Used (LRU) files
  TapeAwareGcLru m_lruQueue;

  //----------------------------------------------------------------------------
  //! @brief Private constructor to enforce only one instance
  //----------------------------------------------------------------------------
  TapeAwareGc();

  //----------------------------------------------------------------------------
  //! Entry point for the GC worker thread
  //----------------------------------------------------------------------------
  void workerThreadEntryPoint() noexcept;

  /// Thrown when a given space cannot be found
  struct SpaceNotFound: public std::runtime_error {
    SpaceNotFound(const std::string &msg): std::runtime_error(msg) {}
  };

  //----------------------------------------------------------------------------
  //! @return Number of free bytes in the specified space
  //!
  //! @param name The name of the space
  //! @throw SpaceNotFound If the specified space annot be found
  //----------------------------------------------------------------------------
  static uint64_t getSpaceNbFreeBytes(const std::string &name);

  //----------------------------------------------------------------------------
  //! Garbage collect
  //!
  //! \return True if a file was garabage collected
  //----------------------------------------------------------------------------
  bool garbageCollect() noexcept;

  //----------------------------------------------------------------------------
  //! Execute stagerrm as user root
  //!
  //! \param fid The file identifier
  //! \return stagerrm result
  //----------------------------------------------------------------------------
  console::ReplyProto stagerrmAsRoot(const FileIdentifier fid);

  //----------------------------------------------------------------------------
  //! Return the preamble to be placed at the beginning of every log message
  //!
  //! @param path file path
  //! @param fid EOS file identifier
  //----------------------------------------------------------------------------
  static std::string createLogPreamble(const std::string &path,
    const FileIdentifier fid);
};

EOSMGMNAMESPACE_END

#endif
