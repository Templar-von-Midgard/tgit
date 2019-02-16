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

add_subdirectory(lib)
add_subdirectory(app)

include(FeatureSummary)
feature_summary(WHAT ALL
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)
