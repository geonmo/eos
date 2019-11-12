# Try to find openssl
# Once done, this will define
#
# OPENSSL_FOUND          - system has openssl
# OPENSSL_INCLUDE_DIRS   - openssl include directories
# OPENSSL_CRYPTO_LIBRARY - openssl crypto library directory
# OPENSSL_CRYPTO_LIBRARY_STATIC - openssl crypto static library directory
#
# and the following imported target
#
# OPENSSL::OPENSSL
# OPENSSL::OPENSSL_STATIC

find_package(PkgConfig)
pkg_check_modules(PC_openssl QUIET openssl)
set(OPENSSL_VERSION ${PC_openssl_VERSION})

find_path(OPENSSL_INCLUDE_DIR
  NAMES openssl/ssl.h
  HINTS ${OPENSSL_ROOT}
  PATH_SUFFIXES include)

find_library(OPENSSL_CRYPTO_LIBRARY
  NAMES crypto
  HINTS ${OPENSSL_ROOT}
  PATH_SUFFIXES ${LIBRARY_PATH_PREFIX})

find_library(OPENSSL_CRYPTO_LIBRARY_STATIC
  NAMES libcrypto.a
  HINTS ${OPENSSL_ROOT}
  PATH_SUFFIXES ${LIBRARY_PATH_PREFIX})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(openssl
  REQUIRED_VARS OPENSSL_CRYPTO_LIBRARY OPENSSL_INCLUDE_DIR OPENSSL_CRYPTO_LIBRARY_STATIC
  VERSION_VAR OPENSSL_VERSION)

mark_as_advanced(OPENSSL_CRYPTO_LIBRARY OPENSSL_INCLUDE_DIR OPENSSL_CRYPTO_LIBRARY_STATIC)

if (OPENSSL_FOUND AND NOT TARGET OPENSSL::OPENSSL AND
    NOT TARGET OPENSSL::OPENSSL_STATIC)
  add_library(OPENSSL::OPENSSL UNKNOWN IMPORTED)
  set_target_properties(OPENSSL::OPENSSL PROPERTIES
    IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}")

  add_library(OPENSSL::OPENSSL_STATIC UNKNOWN IMPORTED)
  set_target_properties(OPENSSL::OPENSSL_STATIC PROPERTIES
    IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY_STATIC}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}")
endif()

set(OPENSSL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
set(OPENSSL_LIBRARIES ${OPENSSL_CRYPTO_LIBRARY})
# Leave this only for qclient to be able to compile
unset(OPENSSL_INCLUDE_DIR)
#unset(OPENSSL_CRYPTO_LIBRARY)
#unset(OPENSSL_CRYPTO_LIBRARY_STATIC)
