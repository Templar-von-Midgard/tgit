cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

add_library(gitpp)
add_classes(gitpp PRIVATE
  Context
  ObjectId
  Repository
  Commit
  Tree
  RevisionWalker
  Diff
  Blob
)
target_link_libraries(gitpp
  PRIVATE git::libgit2
)
target_include_directories(gitpp
  PUBLIC include
  PRIVATE include/gitpp
)
