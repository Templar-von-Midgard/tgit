cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

project(tgit
  VERSION 0.0.1
  DESCRIPTION "A GUI client for git"
  HOMEPAGE_URL "https://github.com/Templar-von-Midgard/tgit"
  LANGUAGES CXX
)
set(PROJECT_DISPLAY_NAME "TGit")

include(Options)
include(tgitSupport)
include(Dependencies)

find_package(libgit2 "${libgit2_VERSION}" EXACT REQUIRED)
find_package(Qt5 5.12 REQUIRED
  COMPONENTS Widgets
             Svg
)

set(DEBUGGER_PATHS
  "$<TARGET_FILE_DIR:Qt5::Core>"
  "$<TARGET_FILE_DIR:git::libgit2>"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  string(APPEND CMAKE_CXX_FLAGS_RELEASE " /Zi")
  foreach(_type "EXE" "MODULE" "SHARED")
    string(APPEND "CMAKE_${_type}_LINKER_FLAGS_RELEASE" " /debug")
  endforeach()
endif()

add_subdirectory(lib)
add_subdirectory(app)

include(FeatureSummary)
feature_summary(WHAT ALL
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)

include(InstallRequiredSystemLibraries)
install(
  FILES "LICENSE"
        "README.md"
  DESTINATION "."
)

set(CPACK_PACKAGE_DISPLAY_NAME "${PROJECT_DISPLAY_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Csonkás Kristóf Gyula")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
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
