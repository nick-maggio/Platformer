# CMakeList.txt : CMake project for Platformer, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.16)
project(Platformer)
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
# Add source to this project's executable.
add_executable (Platformer "Platformer.cpp" "Platformer.h" "animation.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Platformer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

target_link_libraries(Platformer PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

