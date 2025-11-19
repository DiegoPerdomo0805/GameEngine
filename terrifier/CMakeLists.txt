cmake_minimum_required(VERSION 3.16)
project(GAME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib REQUIRED)

set(IMGUI_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(RLIMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/rlImGui)
# set(ENTT_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/external/entt)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

set(IMGUI_SOURCES
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
)

set(RLIMGUI_SOURCES "")
if (EXISTS "${RLIMGUI_DIR}/rlImGui.cpp")
    list(APPEND RLIMGUI_SOURCES "${RLIMGUI_DIR}/rlImGui.cpp")
elseif (EXISTS "${RLIMGUI_DIR}/src/rlImGui.cpp")
    list(APPEND RLIMGUI_SOURCES "${RLIMGUI_DIR}/src/rlImGui.cpp")
else()
    file(GLOB RLIMGUI_GLOB "${RLIMGUI_DIR}/*.cpp" "${RLIMGUI_DIR}/src/*.cpp")
    message(FATAL_ERROR "No se encontró rlImGui.cpp en ${RLIMGUI_DIR}. Archivos vistos: ${RLIMGUI_GLOB}")
endif()
message(STATUS "Usando rlImGui fuente: ${RLIMGUI_SOURCES}")


find_path(ENTT_INCLUDE_DIR entt/entt.hpp
    HINTS
      ${CMAKE_CURRENT_SOURCE_DIR}/external/entt/single_include
      /usr/include
      /usr/local/include
)
if (NOT ENTT_INCLUDE_DIR)
    message(FATAL_ERROR "No se encontró entt/entt.hpp")
endif()
message(STATUS "EnTT include: ${ENTT_INCLUDE_DIR}")


add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${IMGUI_SOURCES}
    ${RLIMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${RLIMGUI_DIR}
    ${ENTT_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} raylib)
# target_compile_features(GAME PRIVATE cxx_std_20)
