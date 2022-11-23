# CMakeList.txt : CMake project for LightHouse, include source and define

cmake_minimum_required(VERSION 3.8)

project("LightHouse")

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path
cmake_path(SET source ${CMAKE_CURRENT_SOURCE_DIR}/src/lighthouse)
cmake_path(SET include ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_path(SET library ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Add source to this project's executable.
add_executable(
	${PROJECT_NAME} 
	${source}/lightHouse.cpp
	${source}/window.cpp
	${source}/output.cpp
	${source}/input.cpp
	${source}/filesystem.cpp
	${source}/engine.cpp
)

# Compile options
if(MSVC)
  target_compile_options(
	  ${PROJECT_NAME} PRIVATE
	  # MSVC
	  /GL /O2 /Oi /Ob3 /W4 /MP /Zf /openmp:experimental /Qpar /Qpar-report:1 /fp:fast /arch:AVX2
  )
endif()

# Preprocessor definitions
add_compile_definitions(
	# vkfw definitions
	VKFW_HPP_NO_NODISCARD_WARNINGS
	VKFW_NO_EXCEPTIONS
	VKFW_NO_LEADING_e_IN_ENUMS
	VKFW_NO_STRUCT_CONSTRUCTORS
	VKFW_NODISCARD
)

remove_definitions(
	# vkfw definitions
	VKFW_NODISCARD
	VKFW_NODISCARD_WHEN_NO_EXCEPTIONS
)

# Include
include_directories(
	${include}
	${include}/lighthouse
	${include}/GLFW
	${include}/glm
	${include}/vulkan
	${include}/vkfw
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
