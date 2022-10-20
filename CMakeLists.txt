# CMakeList.txt : CMake project for LightHouse, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("LightHouse")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source to this project's executable.
add_executable (LightHouse "src/lighthouse/LightHouse.cpp" "include/lighthouse/LightHouse.hpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LightHouse PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
