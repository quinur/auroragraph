cmake_minimum_required(VERSION 3.20)
project(AuroraGraphGUI LANGUAGES CXX)


# Qt6 is discovered in the parent project, so the Qt6::Widgets target should
# already be available.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_add_resources(aurora_gui_rcc resources/aurora.qrc)

add_executable(AuroraGraphGUI
  ${aurora_gui_rcc}
  ../src/gui/main.cpp
  ../src/gui/app.cpp
  ../src/gui/main_window.cpp
  ../src/gui/graph_view.cpp
  ../src/gui/node_item.cpp
  ../src/gui/edge_item.cpp
  ../src/gui/side_panel.cpp
  ../src/gui/query_editor.cpp
  ../src/gui/result_table.cpp
  ../src/gui/status_bar.cpp
  ../src/gui/dialogs.cpp
  ../src/gui/agql_highlighter.cpp
  ../src/gui/job.cpp
  ../src/gui/job_runner.cpp
)

target_include_directories(AuroraGraphGUI PUBLIC
  ${CMAKE_SOURCE_DIR}/gui/include
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(AuroraGraphGUI PRIVATE Qt6::Widgets aurora_core)
