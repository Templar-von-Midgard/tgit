

find_library(LIBGIT2_LIBRARY NAMES libgit2.so libgit2.dll)
find_path(LIBGIT2_INCLUDE_PATH NAMES git2.h)

if(LIBGIT2_INCLUDE_PATH)
  set(_version_regex [[^#define LIBGIT2_VERSION "([0-9]+\.[0-9]+\.[0-9]+)"$]])
  file(STRINGS "${LIBGIT2_INCLUDE_PATH}/git2/version.h" libgit2_VERSION
    REGEX "${_version_regex}"
  )
  string(REGEX REPLACE "${_version_regex}" "\\1" libgit2_VERSION "${libgit2_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libgit2
  REQUIRED_VARS LIBGIT2_LIBRARY LIBGIT2_INCLUDE_PATH
  VERSION_VAR libgit2_VERSION
)

if(libgit2_FOUND)
  add_library(git::libgit2 SHARED IMPORTED)
  set_target_properties(git::libgit2 PROPERTIES
    IMPORTED_LOCATION "${LIBGIT2_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${LIBGIT2_INCLUDE_PATH}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${LIBGIT2_INCLUDE_PATH}"
  )
endif()
