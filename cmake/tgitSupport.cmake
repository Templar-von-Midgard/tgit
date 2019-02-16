include_guard()

function(_add_class target access class)
  get_property(_source_directory
    TARGET "${target}"
    PROPERTY SOURCE_DIR
  )
  if(EXISTS "${_source_directory}/src/${class}.hpp")
    target_sources("${target}" ${access}
      "${_source_directory}/src/${class}.hpp"
    )
  elseif(EXISTS "${_source_directory}/include/${target}/${class}.hpp")
    target_sources("${target}" ${access}
      "${_source_directory}/include/${target}/${class}.hpp"
    )
  else()
    message(FATAL_ERROR
      "Couldn't find a suitable header file for '${class}' in '${target}'"
    )
  endif()
  target_sources("${target}" ${access}
    "${_source_directory}/src/${class}.cpp"
  )
endfunction()

function(add_classes target)
  set(_options "")
  set(_single_args "")
  set(_multi_args "PUBLIC" "PRIVATE" "INTERFACE")
  cmake_parse_arguments(ac "${_options}" "${_single_args}" "${_multi_args}" ${ARGN})
  foreach(_access ${_multi_args})
    foreach(_class ${ac_${_access}})
      _add_class("${target}" ${_access} "${_class}")
    endforeach()
  endforeach()
endfunction()

function(_add_widget target access class)
  _add_class("${target}" "${access}" "${class}")
  get_property(_source_directory
    TARGET "${target}"
    PROPERTY SOURCE_DIR
  )
  if(NOT EXISTS "${_source_directory}/src/${class}.ui")
    message(FATAL_ERROR
      "Couldn't find a suitable ui file for '${class}' in '${target}'"
    )
  endif()
  target_sources("${target}" "${access}"
    "${_source_directory}/src/${class}.ui"
  )
endfunction()

function(add_widgets target)
  set(_options "")
  set(_single_args "")
  set(_multi_args "PUBLIC" "PRIVATE" "INTERFACE")
  cmake_parse_arguments(ac "${_options}" "${_single_args}" "${_multi_args}" ${ARGN})
  foreach(_access ${_multi_args})
    foreach(_class ${ac_${_access}})
      _add_widget("${target}" ${_access} "${_class}")
    endforeach()
  endforeach()
endfunction()
