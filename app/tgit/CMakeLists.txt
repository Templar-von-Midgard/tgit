cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(tgit WIN32
  src/main.cpp
  src/GraphRow.hpp
)
add_classes(tgit
  src/TGitMainWindow
  src/GitLogModel
  src/HistoryModelAdaptor
  src/GraphItemDelegate
  src/CommitView
  src/CommitDetailsWidget
  src/CommitDiffModel
  src/FileDiffWidget
  src/CodeViewer
)
target_link_libraries(tgit
  PRIVATE Qt5::Widgets
          stx
          gitpp
)
target_include_directories(tgit
  PRIVATE src
)
