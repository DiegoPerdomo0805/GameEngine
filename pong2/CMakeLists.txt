cmake_minimum_required(VERSION 3.16)
project(GAME LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Make headers visible to this target
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/entt/single_include
)

target_link_libraries(${PROJECT_NAME} raylib)
