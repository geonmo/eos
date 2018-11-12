//------------------------------------------------------------------------------
// File: BoundIdentityProvider.cc
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

#include "Utils.hh"
#include "BoundIdentityProvider.hh"
#include "EnvironmentReader.hh"
#include <sys/stat.h>

BoundIdentityProvider::BoundIdentityProvider()
: validator(securityChecker)
{
  // create an sss registry
  sssRegistry = new XrdSecsssID(XrdSecsssID::idDynamic);
}

CredentialState
BoundIdentityProvider::fillKrb5FromEnv(const Environment& env, UserCredentials& creds,
                                       uid_t uid, gid_t gid)
{
  creds = UserCredentials::MakeKrb5(
    CredentialFinder::locateKerberosTicket(env),
    uid,
    gid
  );

  TrustedCredentials emptyForNow;
  return validator.validate(creds, emptyForNow);
}

CredentialState
BoundIdentityProvider::fillX509FromEnv(const Environment& env, UserCredentials& creds,
                                       uid_t uid, gid_t gid)
{
  creds = UserCredentials::MakeX509(
    CredentialFinder::locateX509Proxy(env),
    uid,
    gid
  );

  TrustedCredentials emptyForNow;
  return validator.validate(creds, emptyForNow);
}

CredentialState
BoundIdentityProvider::fillSssFromEnv(const Environment& env, UserCredentials& creds,
                                      uid_t uid, gid_t gid)
{
  creds = UserCredentials::MakeSSS(
    CredentialFinder::getSssEndorsement(env),
    uid,
    gid
  );

  return CredentialState::kOk;
}

CredentialState
BoundIdentityProvider::fillCredsFromEnv(const Environment& env,
                                        const CredentialConfig& credConfig,
                                        UserCredentials& creds, uid_t uid, gid_t gid)
{
  if (credConfig.use_user_sss) {
    CredentialState state = fillSssFromEnv(env, creds, uid, gid);

    if (state != CredentialState::kCannotStat) {
      return state;
    }
  }

  // Try krb5 second
  if (credConfig.tryKrb5First) {
    if (credConfig.use_user_krb5cc) {
      CredentialState state = fillKrb5FromEnv(env, creds, uid, gid);

      if (state != CredentialState::kCannotStat) {
        return state;
      }
    }

    if (credConfig.use_user_gsiproxy) {
      CredentialState state = fillX509FromEnv(env, creds, uid, gid);

      if (state != CredentialState::kCannotStat) {
        return state;
      }
    }

    if (credConfig.use_user_sss) {
      CredentialState state = fillSssFromEnv(env, creds, uid, gid);

      if (state != CredentialState::kCannotStat) {
        return state;
      }
    }

    return CredentialState::kCannotStat;
  }

  // Try krb5 second
  if (credConfig.use_user_gsiproxy) {
    CredentialState state = fillX509FromEnv(env, creds, uid, gid);

    if (state != CredentialState::kCannotStat) {
      return state;
    }
  }

  if (credConfig.use_user_krb5cc) {
    CredentialState state = fillKrb5FromEnv(env, creds, uid, gid);

    if (state != CredentialState::kCannotStat) {
      return state;
    }
  }

  return CredentialState::kCannotStat;
}

uint64_t BoundIdentityProvider::getUnixConnectionCounter(uid_t uid, gid_t gid,
    bool reconnect)
{
  std::lock_guard<std::mutex> lock(unixConnectionCounterMtx);

  if (reconnect) {
    unixConnectionCounter[std::make_pair(uid, gid)]++;
  }

  return unixConnectionCounter[std::make_pair(uid, gid)];
}

CredentialState BoundIdentityProvider::unixAuthentication(uid_t uid, gid_t gid,
    pid_t pid, bool reconnect, std::shared_ptr<const BoundIdentity>& result)
{
  LoginIdentifier login(uid, gid, pid, getUnixConnectionCounter(uid, gid,
                        reconnect));
  std::shared_ptr<TrustedCredentials> trustedCreds(new TrustedCredentials());
  result = std::shared_ptr<const BoundIdentity>(new BoundIdentity(login,
           trustedCreds));
  return CredentialState::kOk;
}

//------------------------------------------------------------------------------
// Given a set of user-provided, non-trusted UserCredentials, attempt to
// translate them into a BoundIdentity object. (either by allocating a new
// connection, or re-using a cached one)
//
// If such a thing is not possible, return false.
//------------------------------------------------------------------------------
bool BoundIdentityProvider::userCredsToBoundIdentity(const UserCredentials &creds,
  std::shared_ptr<const BoundIdentity>& result) {

}

CredentialState BoundIdentityProvider::retrieve(const Environment& processEnv,
    uid_t uid, gid_t gid, bool reconnect,
    std::shared_ptr<const BoundIdentity>& result)
{
  UserCredentials credinfo;
  CredentialState state = fillCredsFromEnv(processEnv, credConfig, credinfo, uid, gid);

  if (state != CredentialState::kOk) {
    return state;
  }

  // We found some credentials, yay. We have to bind them to an xrootd
  // connection - does such a binding exist already? We don't want to
  // waste too many LoginIdentifiers, so we re-use them when possible.
  std::shared_ptr<const BoundIdentity> boundIdentity = credentialCache.retrieve(
        credinfo);

  if (boundIdentity && !reconnect) {
    // Cache hit
    result = boundIdentity;
    return CredentialState::kOk;
  }

  if (boundIdentity && boundIdentity->getCreds() && reconnect) {
    // Invalidate credentials
    credentialCache.invalidate(credinfo);
    boundIdentity->getCreds()->invalidate();
  }

  // No binding exists yet, let's create one..
  LoginIdentifier login(connectionCounter++);
  std::shared_ptr<TrustedCredentials> trustedCreds(new TrustedCredentials(
    credinfo));

    // TODO: Conversion from UserCredentials to TrustedCredebtials must be
    // done by the validator.

  // sss credential registration
  if (credinfo.type == CredentialType::SSS) {
    // by default we request the uid/gid name of the calling process
    // the xrootd server rejects to map these if the sss key is not issued for anyuser/anygroup
    XrdSecEntity* newEntity = new XrdSecEntity("sss");
    int errc_uid = 0;
    std::string username = eos::common::Mapping::UidToUserName(uid, errc_uid);
    int errc_gid = 0;
    std::string groupname = eos::common::Mapping::GidToGroupName(gid, errc_gid);

    if (errc_uid) {
      newEntity->name = strdup("nobody");
    } else {
      newEntity->name = strdup(username.c_str());
    }

    if (errc_gid) {
      newEntity->grps = strdup("nogroup");
    } else {
      newEntity->grps = strdup(groupname.c_str());
    }

    // store the endorsement from the environment
    newEntity->endorsements = strdup(credinfo.endorsement.c_str());
    // register new ID
    sssRegistry->Register(login.getStringID().c_str(), newEntity);
  }

  credentialCache.store(credinfo,
    std::unique_ptr<BoundIdentity>(new BoundIdentity(login, trustedCreds)),
    result
  );
  return CredentialState::kOk;
}

CredentialState
BoundIdentityProvider::useDefaultPaths(uid_t uid, gid_t gid, bool reconnect,
                                       std::shared_ptr<const BoundIdentity>& result)
{
  // Pretend as if the environment of the process simply contained the default values,
  // and follow the usual code path.
  Environment defaultEnv;
  defaultEnv.push_back("KRB5CCNAME=FILE:/tmp/krb5cc_" + std::to_string(uid));
  defaultEnv.push_back("X509_USER_PROXY=/tmp/x509up_u" + std::to_string(uid));
  return retrieve(defaultEnv, uid, gid, reconnect, result);
}

CredentialState
BoundIdentityProvider::useGlobalBinding(uid_t uid, gid_t gid, bool reconnect,
                                        std::shared_ptr<const BoundIdentity>& result)
{
  // Pretend as if the environment of the process simply contained the eosfusebind
  // global bindings, and follow the usual code path.
  Environment defaultEnv;
  defaultEnv.push_back(SSTR("KRB5CCNAME=FILE:/var/run/eosd/credentials/uid" << uid
                            << ".krb5"));
  defaultEnv.push_back(SSTR("X509_USER_PROXY=/var/run/eosd/credentials/uid" << uid
                            << ".x509"));
  return retrieve(defaultEnv, uid, gid, reconnect, result);
}

CredentialState
BoundIdentityProvider::retrieve(pid_t pid, uid_t uid, gid_t gid, bool reconnect,
                                std::shared_ptr<const BoundIdentity>& result)
{
  // If not using krb5 or gsi, fallback to unix authentication
  if (!credConfig.use_user_krb5cc && !credConfig.use_user_gsiproxy &&
      !credConfig.use_user_sss) {
    return unixAuthentication(uid, gid, pid, reconnect, result);
  }

  // First, let's read the environment to build up a UserCredentials object.
  Environment processEnv;
  FutureEnvironment response = environmentReader.stageRequest(pid);
  if(!response.waitUntilDeadline(
    std::chrono::milliseconds(credConfig.environ_deadlock_timeout))) {

    eos_static_info("Timeout when retrieving environment for pid %d (uid %d) - we're doing an execve!",
                    pid, uid);
    return {};
  }

  return retrieve(response.get(), uid, gid, reconnect, result);
}

bool BoundIdentityProvider::isStillValid(const BoundIdentity& identity)
{
  if (!identity.getCreds()) {
    return false;
  }

  return identity.getCreds()->isStillValid(securityChecker);
}
