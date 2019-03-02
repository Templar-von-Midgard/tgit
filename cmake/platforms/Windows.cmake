include_guard()

set(PLATFORMS_DIR "${CMAKE_CURRENT_LIST_DIR}")

define_property(TARGET
  PROPERTY DESCRIPTION
  BRIEF_DOCS "Description of this target"
  FULL_DOCS "Description of this target as it should appear in the resource file"
)

function(_add_resource _target)
  set(RESOURCE_TEMPLATE "${PLATFORMS_DIR}/version.rc.in")
  set(_target_resource "${_target}.version.rc")
  message(STATUS "Configuring ${_target_resource} (source: ${RESOURCE_TEMPLATE})")
  set(_options)
  set(_single_args)
  set(_multi_args)
  cmake_parse_arguments(ar "${_options}" "${_single_args}" "${_multi_args}" ${ARGN})

  string(REPLACE "." "," _product_version "${PROJECT_VERSION}")
  string(APPEND _product_version ",0")
  set(_product_version_string "${PROJECT_VERSION}")

  get_target_property(_file_version_string "${_target}" VERSION)
  if(NOT _file_version_string)
    get_target_property(_file_version_string "${_target}" SOVERSION)
  endif()
  if(NOT _file_version_string)
    set(_file_version "${_product_version}")
    set(_file_version_string "${_product_version_string}")
  else()
    string(REPLACE "." "," _file_version "${_file_version_string}")
    string(APPEND _file_version ",0")
  endif()

  set(_comment)
  set(_company_name "${PROJECT_MAINTAINER}")
  get_target_property(_file_description "${_target}" DESCRIPTION)
  if(NOT _file_description)
    set(_file_description "${PROJECT_DESCRIPTION}")
  endif()
  set(_internal_name "${_target}")
  set(_copyright "")
  get_target_property(_suffix "${_target}" SUFFIX)
  if(NOT _suffix)
    get_target_property(_type "${_target}" TYPE)
    set(_suffix "${CMAKE_${_type}_SUFFIX}")
  endif()
  set(_original_filename "${_target}${_suffix}")
  set(_product_name "${PROJECT_NAME}")
  configure_file(
    "${RESOURCE_TEMPLATE}"
    "${_target_resource}"
    @ONLY
  )
  target_sources(${_target} PRIVATE "${_target_resource}")
endfunction()
