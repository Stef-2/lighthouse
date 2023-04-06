cmake_minimum_required(VERSION 3.8)

project("LightHouse")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Path
cmake_path(SET source ${CMAKE_CURRENT_SOURCE_DIR}/source)
cmake_path(SET include ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_path(SET module ${CMAKE_CURRENT_SOURCE_DIR}/module)
cmake_path(SET library ${CMAKE_CURRENT_SOURCE_DIR}/library)

# Add source to this project's executable.
add_executable(
	${PROJECT_NAME} 
	#${module}/lighthouse/vulkan/vulkan.ixx ${source}/../vulkan/vulkan.cpp
	#${module}/std/std.ixx ${source}/../std/std.cpp
	${source}/lighthouse/lightHouse.cpp
	${source}/lighthouse/window.cpp
	${source}/lighthouse/output.cpp
	${source}/lighthouse/input.cpp
	${source}/lighthouse/filesystem.cpp
	${source}/lighthouse/engine.cpp
	${source}/lighthouse/renderer.cpp
	${source}/lighthouse/version.cpp
	${source}/lighthouse/node.cpp
	${source}/vulkan/shaders.cpp
	${source}/vulkan/utils.cpp
	${source}/vulkan/math.cpp
	${source}/lighthouse/system.cpp
	${source}/lighthouse/memory.cpp
	${source}/vulkan/vma_implementation.cpp
	${source}/lighthouse/vulkan/vulkan.cpp
	${source}/lighthouse/vulkan/extension.cpp
	${source}/lighthouse/vulkan/instance.cpp
	${source}/lighthouse/vulkan/debug_messanger.cpp
	${source}/lighthouse/vulkan/physical_device.cpp
	${source}/lighthouse/vulkan/logical_device.cpp
	${source}/lighthouse/vulkan/surface.cpp
)

#STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# Compiler options
if(${CMAKE_CXX_COMPILER_ID} EQUAL MSVC)

  target_compile_options(
	  ${PROJECT_NAME} PRIVATE
	  # MSVC compiler settings
	  /std:c++latest /EHsc /GL /Ox /Oi /W3 /MP /Zf /MD /openmp:experimental /Qpar /Qpar-report:1 /fp:fast /arch:AVX2 /nologo /experimental:module
	  # MSVC disable common annoying warnings
	  /wd4005 /wd4068 /wd4244 /wd4267 /wd4800
	  # Windows.h configuration
	  /DNOMINMAX /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
	)
	
	target_link_options(
		${PROJECT_NAME} PRIVATE
		# MSVC linker settings
		/INCREMENTAL /CGTHREADS:8 /VERBOSE:INCR /LTCG
	)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)

	target_compile_options(
	  ${PROJECT_NAME} PRIVATE
	  #GCC standard compiler settings
	  -C -std=c++23 -fpermissive
	)
	include_directories(
		${PROJECT_NAME} SYSTEM ${INCLUDE}
	)
	
endif()


# Windows specific flags
if(WIN32)
	add_compile_options(
	  ${PROJECT_NAME} PRIVATE
	  /DNOMINMAX /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
)
endif()

# Preprocessor definitions
target_compile_definitions(
	${PROJECT_NAME} PUBLIC

	# vkfw definitions
	VKFW_NO_NODISCARD_WARNINGS
	VKFW_NO_EXCEPTIONS
	VKFW_NO_LEADING_e_IN_ENUMS
	VKFW_NO_STRUCT_CONSTRUCTORS
	VKFW_NO_INCLUDE_VULKAN_HPP

	# vulkan definitions
	VULKAN_HPP_FLAGS_MASK_TYPE_AS_PUBLIC
	VULKAN_HPP_NO_NODISCARD_WARNINGS
	VULKAN_HPP_NO_SPACESHIP_OPERATOR
	VULKAN_HPP_NO_SMART_HANDLE

	# glm definitions
	GLM_ENABLE_EXPERIMENTAL
	GLM_FORCE_INTRINSICS
)

# Include
target_include_directories(
	${PROJECT_NAME} PUBLIC

	${include}
	${include}/lighthouse
	${include}/glfw
	${include}/glm
	${include}/vulkan
	${include}/vulkan/vma
	${include}/vulkan/glslang
	${include}/vkfw
	${include}/lighthouse/vulkan
)

# Modules
set(CMAKE_MODULE_PATH
	${module}/std
	${module}/lighthouse
	${module}/lighthouse/vulkan
)

target_precompile_headers(
	${PROJECT_NAME} PRIVATE
	${include}/vkfw/vkfw.hpp

	${include}/vulkan/vulkan_raii.hpp

	${include}/vulkan/glm/glm.hpp
	${include}/vulkan/glm/ext.hpp
)

# Libraries
target_link_directories(
	${PROJECT_NAME} PRIVATE

	${library}
	${library}/glfw
	${library}/vulkan
)

target_link_libraries(
	${PROJECT_NAME}

	vulkan-1.lib
	glfw3.lib
	SPIRV.lib
	SPIRV-Tools.lib
	SPIRV-Tools-opt.lib
	GenericCodeGen.lib
	MachineIndependent.lib
	OGLCompiler.lib
	OSDependent.lib
)
