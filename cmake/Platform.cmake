include_guard()

if(WIN32)
  include(platforms/Windows)
else()
  include(platforms/Unix)
endif()

macro(add_resource)
  if(WIN32)
    _add_resource(${ARGN})
  endif()
endmacro()
