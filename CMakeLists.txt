cmake_minimum_required(VERSION 3.8)

project("LightHouse")

#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
# Path
cmake_path(SET source ${CMAKE_CURRENT_SOURCE_DIR}/src/lighthouse)
cmake_path(SET include ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_path(SET module ${CMAKE_CURRENT_SOURCE_DIR}/module)
cmake_path(SET library ${CMAKE_CURRENT_SOURCE_DIR}/lib)
#[[
set(CMAKE_CXX_CLANG_TIDY
	clang-tidy
	${source}/*
	${include}/lighthouse/*
	-checks = *
)
]]
# Add source to this project's executable.
add_executable(
	${PROJECT_NAME} 

	${source}/lightHouse.cpp
	${source}/window.cpp
	${source}/output.cpp
	${source}/input.cpp
	${source}/filesystem.cpp
	${source}/engine.cpp
	${source}/renderer.cpp
	${source}/version.cpp
	${source}/node.cpp
	${source}/../vulkan/shaders.cpp
	${source}/../vulkan/utils.cpp
	${source}/../vulkan/math.cpp
	${source}/system.cpp
	${source}/memory.cpp
	${source}/../vulkan/vma_implementation.cpp
 "src/lighthouse/vulkan.cpp")

# Compiler options
if(${CMAKE_CXX_COMPILER_ID} EQUAL MSVC)
  target_compile_options(
	  ${PROJECT_NAME} PRIVATE
	  # MSVC compiler settings
	  /std:c++latest /GL /Ox /Oi /W3 /MP /Zf /openmp:experimental /experimental:module /Qpar /Qpar-report:1 /fp:fast /arch:AVX2 /showIncludes /permissive- /nologo
	  # MSVC disable common annoying warnings
	  /wd4005 /wd4068 /wd4244 /wd4267 /wd4800
	)

	target_link_options(
		${PROJECT_NAME} PRIVATE
		# MSVC linker settings
		/INCREMENTAL /CGTHREADS:8 /VERBOSE:INCR
	)
endif()


if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
message(HERE:${INCLUDE})
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
	VKFW_HPP_NO_NODISCARD_WARNINGS
	VKFW_NO_EXCEPTIONS
	VKFW_NO_LEADING_e_IN_ENUMS
	VKFW_NO_STRUCT_CONSTRUCTORS
	VKFW_NO_INCLUDE_VULKAN_HPP

	# vulkan definitions
	VULKAN_HPP_FLAGS_MASK_TYPE_AS_PUBLIC
	VULKAN_HPP_NO_NODISCARD_WARNINGS
	VULKAN_HPP_NO_SPACESHIP_OPERATOR

	#WIN32 definitions
	NOMINMAX 
)

# Include
target_include_directories(
	${PROJECT_NAME} PRIVATE

	${include}
	${include}/lighthouse
	${include}/glfw
	${include}/glm
	${include}/vulkan
	${include}/vkfw
	${include}/glslang
)

# Modules
set(CMAKE_MODULE_PATH ${module}/std ${module}/lighthouse)

#[[
target_precompile_headers(
	${PROJECT_NAME} PRIVATE
	${include}/vkfw/vkfw.hpp

	${include}/vulkan/vulkan.hpp
	${include}/vulkan/vulkan_enums.hpp
	${include}/vulkan/vulkan_raii.hpp
	${include}/vulkan/vulkan_to_string.hpp
	${include}/vulkan/vulkan_structs.hpp
	${include}/vulkan/vulkan_funcs.hpp
	${include}/vulkan/vulkan_handles.hpp
	${include}/vulkan/vulkan_profiles.hpp
	${include}/vulkan/vulkan_enums.hpp
	${include}/vulkan/vulkan_core.h

	${include}/glm/fwd.hpp
)]]

# Libraries
target_link_directories(
	${PROJECT_NAME}

	PUBLIC ${library}
	PUBLIC ${library}/glfw
	PUBLIC ${library}/vulkan
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
