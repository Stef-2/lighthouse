cmake_minimum_required(VERSION 3.26)

project("lighthouse")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# module support
#set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")


# Path
cmake_path(SET source ${CMAKE_CURRENT_SOURCE_DIR}/source)
cmake_path(SET include ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_path(SET module ${CMAKE_CURRENT_SOURCE_DIR}/module)
cmake_path(SET library ${CMAKE_CURRENT_SOURCE_DIR}/library)
cmake_path(SET dynamic ${CMAKE_CURRENT_SOURCE_DIR}/library)

# Add source to this project's executable.
add_executable(
	${PROJECT_NAME} 
	#${module}/lighthouse/renderer/vulkan/vulkan.ixx ${source}/../vulkan/vulkan.cpp
	#${module}/std/std.ixx ${source}/std/std.cpp
	${include}/lighthouse/window.ixx ${source}/lighthouse/window.cpp
	${include}/lighthouse/static_type.ixx
	${include}/lighthouse/file_system.ixx ${source}/lighthouse/file_system.cpp
	${include}/lighthouse/file_type.ixx
	${include}/lighthouse/utility.ixx
	${include}/lighthouse/datatype.ixx
	${include}/lighthouse/string/string.ixx
	${include}/lighthouse/string/string_convertible.ixx
	${include}/lighthouse/system.ixx
	${include}/lighthouse/output.ixx ${source}/lighthouse/output.cpp
	${include}/lighthouse/input.ixx ${source}/lighthouse/input.cpp
	${include}/lighthouse/operating_system/dynamic_linking.ixx ${source}/lighthouse/operating_system/dynamic_linking.cpp
	${include}/lighthouse/renderer/vulkan/raii_wrapper.ixx
	${include}/lighthouse/renderer/vulkan/utility.ixx
	${include}/lighthouse/renderer/vulkan/index_format.ixx
	${include}/lighthouse/engine.ixx ${source}/lighthouse/engine.cpp
	${source}/lighthouse/renderer/renderer.cpp
	${include}/lighthouse/version.ixx ${source}/lighthouse/version.cpp
	${include}/lighthouse/node.ixx ${source}/lighthouse/node.cpp
	#${source}/vulkan/shaders.cpp
	${source}/vulkan/utils.cpp
	${source}/vulkan/math.cpp
	${source}/lighthouse/operating_system/memory.cpp
	${source}/vulkan/vma_implementation.cpp
	${include}/lighthouse/renderer/vulkan/extension.ixx ${source}/lighthouse/renderer/vulkan/extension.cpp
	${include}/lighthouse/renderer/vulkan/instance.ixx ${source}/lighthouse/renderer/vulkan/instance.cpp
	${include}/lighthouse/renderer/vulkan/debug_messanger.ixx ${source}/lighthouse/renderer/vulkan/debug_messanger.cpp
	${include}/lighthouse/renderer/vulkan/physical_device.ixx ${source}/lighthouse/renderer/vulkan/physical_device.cpp
	${include}/lighthouse/renderer/vulkan/logical_device.ixx ${source}/lighthouse/renderer/vulkan/logical_device.cpp
	${source}/lighthouse/renderer/vulkan/surface.cpp
	${include}/lighthouse/renderer/vulkan/queue_families.ixx ${source}/lighthouse/renderer/vulkan/queue_families.cpp
	${include}/lighthouse/renderer/vulkan/memory_allocator.ixx ${source}/lighthouse/renderer/vulkan/memory_allocator.cpp
	${include}/lighthouse/renderer/vulkan/command_control.ixx ${source}/lighthouse/renderer/vulkan/command_control.cpp
	${include}/lighthouse/renderer/vulkan/queue.ixx ${source}/lighthouse/renderer/vulkan/queue.cpp
	${source}/lighthouse/renderer/vulkan/image.cpp
	${source}/lighthouse/renderer/vulkan/swapchain.cpp
	${source}/lighthouse/renderer/vulkan/shader_module.cpp
	${include}/lighthouse/renderer/vulkan/descriptor_set_layout.ixx ${source}/lighthouse/renderer/vulkan/descriptor_set_layout.cpp
	${include}/lighthouse/renderer/vulkan/buffer.ixx ${source}/lighthouse/renderer/vulkan/buffer.cpp
	${include}/lighthouse/renderer/vulkan/sampler.ixx ${source}/lighthouse/renderer/vulkan/sampler.cpp
	${include}/lighthouse/renderer/vulkan/descriptor_buffer.ixx ${source}/lighthouse/renderer/vulkan/descriptor_buffer.cpp
	${source}/lighthouse/renderer/vulkan/spir_v.cpp
	${source}/lighthouse/renderer/vulkan/shader_object.cpp
	${source}/lighthouse/renderer/vulkan/vertex_buffer.cpp
	${source}/lighthouse/renderer/vulkan/shader_object_pipeline.cpp
	${source}/lighthouse/renderer/scene_loader.cpp
	${include}/lighthouse/renderer/vulkan/pipeline_resource_generator.ixx ${source}/lighthouse/renderer/vulkan/pipeline_resource_generator.cpp
	${source}/lighthouse/renderer/vulkan/shader_input.cpp
	${source}/lighthouse/renderer/bounding_volume.cpp
	${source}/lighthouse/renderer/mesh.cpp
	${include}/lighthouse/renderer/bounding_volume.ixx ${source}/lighthouse/renderer/bounding_volume.cpp
	${source}/lighthouse/lightHouse.cpp
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
	  /FIC:/Users/Stefan/source/repos/lighthouse/out/build/MSVC-x64-release/CMakeFiles/lighthouse.dir/RelWithDebInfo/cmake_pch.hxx
	)
	
	target_link_options(
		${PROJECT_NAME} PRIVATE
		# MSVC linker settings
		/INCREMENTAL /CGTHREADS:8 /VERBOSE:INCR /LTCG /${library}/std/std.obj
		# No console
		#/SUBSYSTEM:windows /ENTRY:mainCRTStartup
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
	#VULKAN_HPP_NO_SMART_HANDLE
	#VULKAN_HPP_NO_SETTERS

	# glm definitions
	GLM_ENABLE_EXPERIMENTAL
	GLM_FORCE_INTRINSICS

	# project specific
	DYNAMIC_LINK_DIRECTORY="${dynamic}"
	INTELLISENSE=__INTELLISENSE__
	DONT_OPTIMIZE=__pragma\(optimize\(\"\",off\)\)
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
	$ENV{IFCPATH}/Release
)

# Modules
set(CMAKE_MODULE_PATH
	${module}/std
	${module}/lighthouse
	${module}/lighthouse/renderer/vulkan
)

target_precompile_headers(
	${PROJECT_NAME} PRIVATE
	${include}/vkfw/vkfw.hpp

	${include}/vulkan/vulkan_raii.hpp
	${include}/vulkan/vma/vk_mem_alloc.hpp

	${include}/vulkan/glm/glm.hpp
	${include}/vulkan/glm/ext.hpp
)

# Libraries
target_link_directories(
	${PROJECT_NAME} PRIVATE

	${library}
	${library}/glfw
	${library}/vulkan
	${library}/assimp
	${library}/std
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

	spirv-cross-core
	spirv-cross-cpp.lib
	spirv-cross-glsl.lib
	spirv-cross-reflect.lib

	assimp-vc143-mt.lib

	#std.obj
)