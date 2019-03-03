message(STATUS "${CPACK_TOPLEVEL_DIRECTORY}")
message(STATUS "${CPACK_TEMPORARY_DIRECTORY}")

set(_bin_dir "${CPACK_TEMPORARY_DIRECTORY}/bin")

set(_is_uninstall_subkey "Microsoft\\Windows\\CurrentVersion\\Uninstall\\Inno Setup 5_is1")
find_program(iscc_EXE
  NAMES ISCC
  PATHS
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\${_is_uninstall_subkey};InstallLocation]"
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\${_is_uninstall_subkey};InstallLocation]"
)

if(NOT iscc_EXE)
  message(FATAL_ERROR "Couldn't find Inno Setup")
endif()

set(disabled_features "compiler-runtime" "translations" "system-d3d-compiler")
list(TRANSFORM disabled_features PREPEND "--no-")
execute_process(
  COMMAND "${windeployqt_EXE}" "--pdb" ${disabled_features} "${_bin_dir}"
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

set(_components)
get_filename_component(_readme_filename "${CPACK_RESOURCE_FILE_README}" NAME)
set(_files_section "[Files]\n")
foreach(_file ${_files})
  get_filename_component(_dest_dir "${_file}" DIRECTORY)
  file(TO_NATIVE_PATH "${_dest_dir}" _dest_dir)
  if(_dest_dir)
    string(PREPEND _dest_dir "\\")
  endif()
  string(APPEND _files_section
    "Source: \"${_file}\"; DestDir: \"{app}${_dest_dir}\""
  )
  get_filename_component(_ext "${_file}" EXT)
  if(_ext STREQUAL ".pdb")
    string(APPEND _files_section
      "; Components: pdb"
    )
    if(NOT "pdb" IN_LIST _components)
      list(APPEND _components "pdb")
    endif()
  endif()
  get_filename_component(_filename "${_file}" NAME)
  if(_filename STREQUAL _readme_filename)
    string(APPEND _files_section "; Flags: isreadme")
  endif()
  string(APPEND _files_section "\n")
endforeach()

if(_components)
  set(_components_section "[Components]\n")
  if("pdb" IN_LIST _components)
    string(APPEND _components_section
      "Name: \"pdb\"; Description: \"Debugging Information\"; Types: full\n"
    )
  endif()
endif()

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/TGitSetup.iss.in"
  "${CPACK_TOPLEVEL_DIRECTORY}/TGitSetup.iss"
  @ONLY
)

execute_process(
  COMMAND "${iscc_EXE}"
          "/O${CPACK_OUTPUT_FILE_PREFIX}"
          "${CPACK_TOPLEVEL_DIRECTORY}/TGitSetup.iss"
)
