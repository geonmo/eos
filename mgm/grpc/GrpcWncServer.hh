//-----------------------------------------------------------------------------
// File: GrpcWncServer.hh
// Author: Branko Blagojevic <branko.blagojevic@comtrade.com>
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------
#include "mgm/Namespace.hh"
#include "common/AssistedThread.hh"
#include "common/Logging.hh"
#include "GrpcWncInterface.hh"
//-----------------------------------------------------------------------------
#ifdef EOS_GRPC
#include <grpc++/grpc++.h>
#endif
//-----------------------------------------------------------------------------

EOSMGMNAMESPACE_BEGIN

/**
 * @file   GrpcWncServer.hh
 *
 * @brief  This class implements a gRPC server for EOS Windows native client
 * running embedded in the MGM
 *
 */
class GrpcWncServer
{
private:
  int mWncPort; // 50052 by default
  bool mSSL;
  std::string mSSLCert;
  std::string mSSLKey;
  std::string mSSLCa;
  std::string mSSLCertFile;
  std::string mSSLKeyFile;
  std::string mSSLCaFile;
  AssistedThread mThread; // Thread running gRPC service

#ifdef EOS_GRPC
  std::unique_ptr<grpc::Server> mWncServer;
#endif

public:

  // Default Constructor - enabling port 50052 by default
  GrpcWncServer(int port = 50052) : mWncPort(port), mSSL(false) { }

  ~GrpcWncServer()
  {
#ifdef EOS_GRPC

    if (mWncServer) {
      eos_static_info("%s", "msg=\"stopping gRPC server for EOS-wnc\"");
      mWncServer->Shutdown();
    }

#endif
    mThread.join();
  }

  // Run gRPC server for EOS Windows native client
  void RunWnc(ThreadAssistant& assistant) noexcept;

  // Create thread for gRPC server for EOS Windows native client
  void StartWnc()
  {
    mThread.reset(&GrpcWncServer::RunWnc, this);
  }
};

EOSMGMNAMESPACE_END
