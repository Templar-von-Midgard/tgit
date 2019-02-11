cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

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

project(tgit VERSION 0.1.0 LANGUAGES CXX)

find_package(libgit2 REQUIRED)
find_package(Qt5 REQUIRED
  COMPONENTS Widgets
)

add_subdirectory(app)
