cmake_minimum_required(VERSION 3.16)
project(GAME VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_path(RAYLIB_INCLUDE_DIR raylib.h
    HINTS
      /usr/include
      /usr/local/include
)



# Raylib
find_library(RAYLIB_LIBRARY NAMES raylib
    HINTS
      /usr/lib
      /usr/lib/x86_64-linux-gnu
      /usr/local/lib
)

if (NOT RAYLIB_INCLUDE_DIR)
    message(FATAL_ERROR "raylib header not found (raylib.h). Is libraylib5-dev installed?")
endif()

if (NOT RAYLIB_LIBRARY)
    message(FATAL_ERROR "raylib library not found (libraylib.so). Is libraylib5-dev installed?")
endif()


# imgui

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



# rlimgui
message(${RLIMGUI_DIR})
file(GLOB CONTENTS "${RLIMGUI_DIR}/*")

list(LENGTH CONTENTS NUM_ITEMS)

if(NUM_ITEMS EQUAL 0)
    message(STATUS "Directory '${RLIMGUI_DIR}' is empty.")
endif()


file(GLOB_RECURSE RLIMGUI_SOURCES
    "${RLIMGUI_DIR}/rlImGui.cpp"
)

if (NOT RLIMGUI_SOURCES)
    file(GLOB_RECURSE RLIMGUI_GLOB "${RLIMGUI_DIR}/*.cpp")
    message(FATAL_ERROR 
        "No se encontró rlImGui.cpp en ${RLIMGUI_DIR}. Archivos vistos: ${RLIMGUI_GLOB}")
endif()
message(STATUS "Usando rlImGui fuente: ${RLIMGUI_SOURCES}")


# Entt
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
    ${RAYLIB_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${RAYLIB_LIBRARY}
)
# target_compile_features(GAME PRIVATE cxx_std_20)
