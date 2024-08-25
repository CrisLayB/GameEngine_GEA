cmake_minimum_required(VERSION 3.28)
project(game)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
#anadirlo entt a este directorio
#include_directories(/usr/local/include/entt)


file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(
  game
  ${SOURCE_FILES}
)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

target_include_directories(game  
  PUBLIC src
  PUBLIC ${SDL2_INCLUDE_DIRS}
  PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(
  game
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
)
