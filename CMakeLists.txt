cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

project(tgit VERSION 0.0.1 LANGUAGES CXX)

include(Options)
include(tgitSupport)

find_package(libgit2 0.27 REQUIRED)
find_package(Qt5 5.12 REQUIRED
  COMPONENTS Widgets
)

set(DEBUGGER_PATHS
  "$<TARGET_FILE_DIR:Qt5::Core>"
  "$<TARGET_FILE_DIR:git::libgit2>"
)

add_subdirectory(lib)
add_subdirectory(app)

include(FeatureSummary)
feature_summary(WHAT ALL
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)

include(InstallRequiredSystemLibraries)

if(WIN32)
  set(CPACK_GENERATOR "External")
  set(CPACK_EXTERNAL_REQUESTED_VERSIONS "1.0")
  set(CPACK_EXTERNAL_ENABLE_STAGING ON)
  set(CPACK_EXTERNAL_PACKAGE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/InnoSetupPackaging.cmake")
  get_target_property(Qt5_QMAKE_LOCATION Qt5::qmake IMPORTED_LOCATION)
  get_target_property(libgit2_LOCATION_RELEASE git::libgit2 IMPORTED_LOCATION_RELEASE)
  if(NOT libgit2_LOCATION_RELEASE)
    get_target_property(libgit2_LOCATION_RELEASE git::libgit2 IMPORTED_LOCATION)
  endif()
  get_target_property(libgit2_LOCATION_DEBUG git::libgit2 IMPORTED_LOCATION_DEBUG)
  if(NOT libgit2_LOCATION_DEBUG)
    get_target_property(libgit2_LOCATION_DEBUG git::libgit2 IMPORTED_LOCATION)
  endif()
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPackProjectConfigInnoSetup.cmake.in"
    "CPackProjectConfigInnoSetup.cmake"
    @ONLY
  )
  set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CPackProjectConfigInnoSetup.cmake")
endif()
include(CPack)
