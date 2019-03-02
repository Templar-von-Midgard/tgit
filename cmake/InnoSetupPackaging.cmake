message(STATUS "${CPACK_TOPLEVEL_DIRECTORY}")
message(STATUS "${CPACK_TEMPORARY_DIRECTORY}")

set(_bin_dir "${CPACK_TEMPORARY_DIRECTORY}/bin")

 get_directory_property(_variables VARIABLES)
 foreach(_var ${_variables})
   message(STATUS "${_var}: ${${_var}}")
 endforeach()

set(disabled_features "compiler-runtime" "translations" "system-d3d-compiler")
list(TRANSFORM disabled_features PREPEND "--no-")
execute_process(
  COMMAND "${windeployqt_EXE}" ${disabled_features} "${_bin_dir}"
)
if(CPACK_BUILD_CONFIG STREQUAL "Release")
  file(INSTALL "${libgit2_dll_release}"
    DESTINATION "${_bin_dir}"
  )
else()
  file(INSTALL "${libgit2_dll_debug}"
    DESTINATION "${_bin_dir}"
  )
endif()

file(GLOB_RECURSE _installed_paths
  LIST_DIRECTORIES true
  "${CPACK_TEMPORARY_DIRECTORY}/*"
)

set(_directories)
set(_files)
foreach(_path ${_installed_paths})
  file(RELATIVE_PATH _relative_path "${CPACK_TEMPORARY_DIRECTORY}" "${_path}")
  file(TO_NATIVE_PATH "${_relative_path}" _relative_path)
  if(IS_DIRECTORY "${_path}")
    list(APPEND _directories "${_relative_path}")
  else()
    list(APPEND _files "${_relative_path}")
  endif()
endforeach()

set(_dirs_section "[Dirs]\n")
foreach(_dir ${_directories})
  string(APPEND _dirs_section "Name: \"{app}\\${_dir}\"\n")
endforeach()

set(_files_section "[Files]\n")
foreach(_file ${_files})
  get_filename_component(_dest_dir "${_file}" DIRECTORY)
  file(TO_NATIVE_PATH "${_dest_dir}" _dest_dir)
  if(_dest_dir)
    string(PREPEND _dest_dir "\\")
  endif()
  string(APPEND _files_section
    "Source: \"${_file}\"; DestDir: \"{app}${_dest_dir}\"\n"
  )
endforeach()

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/TGitSetup.iss.in"
  "${CPACK_TEMPORARY_DIRECTORY}/TGitSetup.iss"
  @ONLY
)

execute_process(
  COMMAND "C:\\Program Files (x86)\\Inno Setup 5\\ISCC.exe"
          "/O${CPACK_PACKAGE_DIRECTORY}"
          "TGitSetup.iss"
  WORKING_DIRECTORY "${CPACK_TEMPORARY_DIRECTORY}"
)
