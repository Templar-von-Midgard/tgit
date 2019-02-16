cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

macro(add_classes _target)
  set(_sources)
  foreach(_src ${ARGN})
    list(APPEND _sources ${_src}.cpp ${_src}.hpp)
    if(EXISTS ${_src}.ui OR EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_src}.ui)
      list(APPEND _sources ${_src}.ui)
    endif()
  endforeach()
  message(STATUS "${_sources}")
  target_sources(${_target}
    PRIVATE ${_sources}
  )
  unset(_sources)
endmacro()

project(tgit VERSION 0.0.1 LANGUAGES CXX)

include(Options)

find_package(libgit2 0.27 REQUIRED)
find_package(Qt5 5.12 REQUIRED
  COMPONENTS Widgets
)

add_subdirectory(lib)
add_subdirectory(app)

include(FeatureSummary)
feature_summary(WHAT ALL)
