cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set(SOURCES
  include/gitpp/Context.hpp
  src/Context.cpp
  include/gitpp/ObjectId.hpp
  src/ObjectId.cpp
  include/gitpp/Repository.hpp
  src/Repository.cpp
  include/gitpp/Commit.hpp
  src/Commit.cpp
  include/gitpp/Tree.hpp
  src/Tree.cpp
  include/gitpp/RevisionWalker.hpp
  src/RevisionWalker.cpp
  include/gitpp/Diff.hpp
  src/Diff.cpp
)

add_library(gitpp ${SOURCES})
target_link_libraries(gitpp
  PRIVATE git::libgit2
)
target_include_directories(gitpp
  PUBLIC include
  PRIVATE include/gitpp
)
