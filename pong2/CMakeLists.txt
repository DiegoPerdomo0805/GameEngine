cmake_minimum_required(VERSION 3.16)
project(GAME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

set(IMGUI_DIR  ${PROJECT_SOURCE_DIR}/external/external/imgui)
set(RLIMGUI_DIR ${PROJECT_SOURCE_DIR}/external/external/rlImGui)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Dear ImGui + rlImGui sources compiled into the executable
set(IMGUI_SOURCES
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  # You can include this to see the ImGui demo window (optional):
  # ${IMGUI_DIR}/imgui_demo.cpp
  ${RLIMGUI_DIR}/rlImGui.cpp
)

add_executable(${PROJECT_NAME}
  ${SOURCES}
  ${IMGUI_SOURCES}
)

# Header search paths (your ECS headers, entt, imgui, rlImGui)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/external/entt/single_include
  ${IMGUI_DIR}
  ${RLIMGUI_DIR}
)

target_link_libraries(${PROJECT_NAME} raylib)
