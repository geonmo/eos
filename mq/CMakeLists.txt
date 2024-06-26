#-------------------------------------------------------------------------------
# File: CMakeLists.txt
# Author: Andreas-Joachim Peters - CERN
#-------------------------------------------------------------------------------

# ************************************************************************
# * EOS - the CERN Disk Storage System                                   *
# * Copyright (C) 2011 CERN/Switzerland                                  *
# *                                                                      *
# * This program is free software: you can redistribute it and/or modify *
# * it under the terms of the GNU General Public License as published by *
# * the Free Software Foundation, either version 3 of the License, or    *
# * (at your option) any later version.                                  *
# *                                                                      *
# * This program is distributed in the hope that it will be useful,      *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
# * GNU General Public License for more details.                         *
# *                                                                      *
# * You should have received a copy of the GNU General Public License    *
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
# ************************************************************************

include_directories(
  ${CMAKE_SOURCE_DIR}/namespace/ns_quarkdb/qclient/include)

#-------------------------------------------------------------------------------
# XrdMqClient-Objects
#-------------------------------------------------------------------------------
add_library(XrdMqClient-Objects OBJECT
  XrdMqTiming.hh
  QdbListener.cc                 QdbListener.hh
  ErrorReportListener.cc         ErrorReportListener.hh
  FsChangeListener.cc            FsChangeListener.hh
  GlobalConfigChangeListener.cc  GlobalConfigChangeListener.hh
  MessagingRealm.cc              MessagingRealm.hh
  ReportListener.cc              ReportListener.hh
  SharedDequeProvider.cc         SharedDequeProvider.hh
  SharedHashProvider.cc          SharedHashProvider.hh
  SharedHashWrapper.cc           SharedHashWrapper.hh
  LocalHash.cc                   LocalHash.hh
  XrdMqClient.cc                 XrdMqClient.hh
  XrdMqMessage.cc                XrdMqMessage.hh
  XrdMqMessaging.cc              XrdMqMessaging.hh
  XrdMqSharedObject.cc           XrdMqSharedObject.hh)

target_link_libraries(XrdMqClient-Objects PUBLIC
  XROOTD::UTILS
  PROTOBUF::PROTOBUF
  OpenSSL::SSL
  GOOGLE::SPARSEHASH
  qclient)

set_target_properties(XrdMqClient-Objects PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE)

#-------------------------------------------------------------------------------
# XrdMqClient and XrdMqClient-Static library
#-------------------------------------------------------------------------------
add_library(XrdMqClient SHARED
  $<TARGET_OBJECTS:XrdMqClient-Objects>)

target_link_libraries(XrdMqClient PUBLIC
  XrdMqClient-Objects
  EosCommon)

set_target_properties(
  XrdMqClient PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR}
  MACOSX_RPATH TRUE)

install(TARGETS XrdMqClient
  LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

add_library(XrdMqClient-Static STATIC
  $<TARGET_OBJECTS:XrdMqClient-Objects>)

target_link_libraries(XrdMqClient-Static PUBLIC
  XrdMqClient-Objects
  EosCommon-Static)

set_target_properties(XrdMqClient-Static PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE)

#-------------------------------------------------------------------------------
# XrdMqOfs library
#-------------------------------------------------------------------------------
if (NOT CLIENT)
add_library(XrdMqOfs-${XRDPLUGIN_SOVERSION} MODULE
  XrdMqOfs.cc       XrdMqOfs.hh
  XrdMqMessage.cc   XrdMqMessage.hh)

target_link_libraries(XrdMqOfs-${XRDPLUGIN_SOVERSION} PRIVATE
  EosCommon
  qclient)

install(TARGETS XrdMqOfs-${XRDPLUGIN_SOVERSION}
  LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
endif ()
