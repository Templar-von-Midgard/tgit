cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

project(tgit VERSION 0.1.0 LANGUAGES CXX)

find_package(libgit2 REQUIRED)
find_package(Qt5 REQUIRED
  COMPONENTS Widgets
)

add_subdirectory(app)
