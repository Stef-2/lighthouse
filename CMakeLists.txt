# CMakeList.txt : CMake project for LightHouse, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("LightHouse")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source to this project's executable.
add_executable(
	${PROJECT_NAME} 
	
	src/lighthouse/LightHouse.cpp
	src/lighthouse/Window.cpp
 "src/lighthouse/Output.cpp")

# Include
include_directories(
	"include",
	"include/lighthouse",
	"include/GLFW",
	"include/glm",
	"include/vulkan"
)

# Libraries
target_link_directories(
	${PROJECT_NAME}

	PUBLIC ${CMAKE_SOURCE_DIR}/lib,
	PUBLIC ${CMAKE_SOURCE_DIR}/lib/glfw,
	PUBLIC ${CMAKE_SOURCE_DIR}/lib/vulkan
)

target_link_libraries(
	${PROJECT_NAME}

	vulkan-1.lib
	glfw3.lib
)
