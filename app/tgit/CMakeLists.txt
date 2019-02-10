cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set(CMAKE_AUTOMOC ON)

add_executable(tgit WIN32
  src/main.cpp
)
add_classes(tgit
  src/TGitMainWindow
  src/GitLogModel
)
target_link_libraries(tgit
  PRIVATE git::libgit2
          Qt5::Widgets
)
