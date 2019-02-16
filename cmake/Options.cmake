
get_property(_is_multiconfig
  GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG
)

set(_supported_configurations "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
if(NOT _is_multiconfig)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      ""
    )
  elseif(NOT CMAKE_BUILD_TYPE IN_LIST _supported_configurations)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      ""
      FORCE
    )
  endif()
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS ${_supported_configurations}
  )
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY HELPSTRING "Build configuration (Default: 'Debug')"
  )
else()
  set(CMAKE_CONFIGURATION_TYPES ${_supported_configurations} CACHE STRING
    "Supported configuration types"
    FORCE
  )
endif()

set(_asan_support "GNU" "Clang")
if(CMAKE_CXX_COMPILER_ID IN_LIST _asan_support)
  option(USE_ASAN "Enable AddressSanitizer" OFF)
endif()

set(_ubsan_support "Clang")
if(CMAKE_CXX_COMPILER_ID IN_LIST _ubsan_support)
  option(USE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
endif()

set(_sanitizers)
if(USE_ASAN)
  list(APPEND _sanitizers "address")
endif()
if(USE_UBSAN)
  list(APPEND _sanitizers "undefined")
endif()

if(_sanitizers)
  list(JOIN _sanitizers "," _sanitizer_flags)
  add_compile_options("-fsanitize=${_sanitizer_flags}")
  add_link_options("-fsanitize=${_sanitizer_flags}")
endif()

option(USE_IWYU "Enable include-what-you-use" OFF)
if(USE_IWYU)
  if(NOT CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
    find_program(_iwyu NAMES include-what-you-use)
    mark_as_advanced(_iwyu)
    if(NOT _iwyu)
      message(FATAL_ERROR
        "include-what-you-use couldn't be found, provide it via the 'CMAKE_CXX_INCLUDE_WHAT_YOU_USE' cache variable"
      )
    endif()
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "${_iwyu}" CACHE PATH
      "Path to include-what-you-use"
    )
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
  CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options("-W" "-Wall" "-Wextra" "-pedantic" "-Werror")
endif()
