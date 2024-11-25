cmake_minimum_required(VERSION 3.28)
project(game)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

# Include FMOD headers
include_directories(${PROJECT_SOURCE_DIR}/include/fmod)

# Link FMOD library directory
link_directories(${PROJECT_SOURCE_DIR}/lib/fmod)

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
set(FMOD_INCLUDE_DIR "/usr/include/fmod")
set(FMOD_LIBRARIES "/usr/lib/libfmod.so")

target_include_directories(game  
  PUBLIC src
  PUBLIC ${SDL2_INCLUDE_DIRS}
  PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS}
  PUBLIC ${BOX2D_INCLUDE_DIRS}
  PUBLIC ${FMOD_INCLUDE_DIRS}
)

# Link SDL2, SDL2_image, and FMOD to the executable
target_link_libraries(
  game
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  ${FMOD_LIBRARY}
  ${FMOD_LIBRARIES}
)
