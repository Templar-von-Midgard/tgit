cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(tgit WIN32
  src/main.cpp
  src/GraphRow.hpp
  src/GitUtils.hpp
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
  src/DiffView
  src/CodeViewer
)
target_link_libraries(tgit
  PRIVATE git::libgit2
          Qt5::Widgets
          stx
          gitpp
)
target_include_directories(tgit
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)
