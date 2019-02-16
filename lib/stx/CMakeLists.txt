cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

add_library(stx src/dummy.cpp)
target_sources(stx PRIVATE
  include/stx/overload.hpp
)
target_include_directories(stx
  PUBLIC include
)
