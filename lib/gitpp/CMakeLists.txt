cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set(SOURCES
  include/gitpp/ObjectId.hpp
  src/ObjectId.cpp
)

add_library(gitpp ${SOURCES})
target_link_libraries(gitpp
  PRIVATE git::libgit2
)
target_include_directories(gitpp
  PUBLIC include
  PRIVATE include/gitpp
)
