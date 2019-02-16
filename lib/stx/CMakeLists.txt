cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

add_library(stx)
target_sources(stx PRIVATE
  src/dummy.cpp
  include/stx/overload.hpp
)
target_include_directories(stx
  PUBLIC include
)
