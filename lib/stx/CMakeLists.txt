cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

add_library(stx)
target_sources(stx PRIVATE
  src/dummy.cpp
  include/stx/overload.hpp
  include/stx/utility/macros.hpp
  include/stx/type_traits/compare.hpp
  include/stx/algorithm/find.hpp
  include/stx/algorithm/contains.hpp
  include/stx/algorithm/index_of.hpp
)
target_include_directories(stx
  PUBLIC include
)
