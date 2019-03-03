

find_library(libgit2_LIBRARY
  NAMES git2
)
find_path(libgit2_INCLUDE_DIR
  NAMES git2.h
)

if(libgit2_INCLUDE_DIR)
  set(_version_regex [[^#define LIBGIT2_VERSION "([0-9]+\.[0-9]+\.[0-9]+)"$]])
  file(STRINGS "${libgit2_INCLUDE_DIR}/git2/version.h" libgit2_VERSION
    REGEX "${_version_regex}"
  )
  string(REGEX REPLACE "${_version_regex}" "\\1" libgit2_VERSION "${libgit2_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libgit2
  REQUIRED_VARS libgit2_LIBRARY
                libgit2_INCLUDE_DIR
  VERSION_VAR libgit2_VERSION
)

if(libgit2_FOUND)
  add_library(git::libgit2 SHARED IMPORTED)
  set_target_properties(git::libgit2 PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    INTERFACE_INCLUDE_DIRECTORIES "${libgit2_INCLUDE_DIR}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${libgit2_INCLUDE_DIR}"
  )
  if(WIN32)
    get_filename_component(libgit2_LIBRARY_DIR "${libgit2_LIBRARY}" DIRECTORY)
    get_filename_component(libgit2_LOCATION "${libgit2_LIBRARY_DIR}/../bin/git2.dll" ABSOLUTE)
    set_target_properties(git::libgit2 PROPERTIES
      IMPORTED_IMPLIB "${libgit2_LIBRARY}"
      IMPORTED_LOCATION "${libgit2_LOCATION}"
    )
  else()
    set_target_properties(git::libgit2 PROPERTIES
      IMPORTED_LOCATION "${libgit2_LIBRARY}"
    )
  endif()
endif()
