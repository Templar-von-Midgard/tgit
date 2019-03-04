include_guard()

include(FetchContent)

FetchContent_Declare(libssh2
  # GIT_REPOSITORY "https://github.com/libssh2/libssh2"
  # GIT_TAG "libssh2-1.8.0"
  URL "https://github.com/libssh2/libssh2/archive/libssh2-1.8.0.tar.gz"
  URL_HASH "SHA256=973f63f98141d68b4a1bc85791ee29411eeab12a6230ae1aca9c368550ccafae"
)
FetchContent_GetProperties(libssh2)
if(NOT libssh2_POPULATED)
  FetchContent_Populate(libssh2)
endif()

FetchContent_Declare(libgit2
  # GIT_REPOSITORY "https://github.com/libgit2/libgit2"
  # GIT_TAG "v0.27.8"
  URL "https://github.com/libgit2/libgit2/archive/v${libgit2_VERSION}.tar.gz"
  URL_HASH "${libgit2_CHECKSUM}"
)
FetchContent_GetProperties(libgit2)
if(NOT libgit2_POPULATED)
  FetchContent_Populate(libgit2)
  set(libgit2_ROOT "${FETCHCONTENT_BASE_DIR}/libgit2-install")
  set(libgit2_CACHE_ARGS
    "-DBUILD_CLAR=OFF"
    "-DEMBED_SSH_PATH=${libssh2_SOURCE_DIR}"
    "-DCMAKE_INSTALL_PREFIX=${libgit2_ROOT}"
  )
  if(CMAKE_BUILD_TYPE)
    list(APPEND libgit2_CACHE_ARGS "-DCMAKE_BUILD_TYPE=${libgit2_CONFIG}")
  endif()
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" ${libgit2_CACHE_ARGS} "${libgit2_SOURCE_DIR}"
    WORKING_DIRECTORY "${libgit2_BINARY_DIR}"
  )
  if(CMAKE_BUILD_TYPE)
    execute_process(
      COMMAND "${CMAKE_COMMAND}" --build "${libgit2_BINARY_DIR}" --target install
    )
  else()
    execute_process(
      COMMAND "${CMAKE_COMMAND}" --build "${libgit2_BINARY_DIR}" --config "${libgit2_CONFIG}" --target install
    )
  endif()
endif()
