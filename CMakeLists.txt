cmake_minimum_required(VERSION 3.27.1)

project("lighthouse")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# module support
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
#set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")
add_compile_options(/ifcSearchDir C:/Users/Stefan/source/repos/lighthouse/build/MSVC-x64-release/CMakeFiles/lighthouse.dir)


# Path
cmake_path(SET source ${CMAKE_CURRENT_SOURCE_DIR}/source)
cmake_path(SET include ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_path(SET module ${CMAKE_CURRENT_SOURCE_DIR}/module)
cmake_path(SET library ${CMAKE_CURRENT_SOURCE_DIR}/library)
cmake_path(SET dynamic ${CMAKE_CURRENT_SOURCE_DIR}/library)

# std module
add_library(std_module)
target_sources(std_module PUBLIC FILE_SET CXX_MODULES FILES
	${module}/std/std.ixx)
target_compile_options(std_module PUBLIC
	/std:c++latest /EHsc /GL /Ox /Oi /W3 /MP /Zf /MD /Qpar /Qpar-report:1 /arch:AVX2 /nologo#/translateInclude
	/DNOMINMAX /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
)
# std.compat module
add_library(std_compat_module)
target_sources(std_compat_module PUBLIC FILE_SET CXX_MODULES FILES
	${module}/std/std.compat.ixx ${module}/std/std.ixx)
target_compile_options(std_compat_module PUBLIC
	/std:c++latest /EHsc /GL /Ox /Oi /W3 /MP /Zf /MD /Qpar /Qpar-report:1 /arch:AVX2 /nologo#/translateInclude
	/DNOMINMAX /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
)
# vulkan module
add_library(vulkan_module)
target_sources(vulkan_module PUBLIC FILE_SET CXX_MODULES FILES
	${include}/vulkan/vulkan.cppm
)
target_compile_options(vulkan_module PUBLIC
	/std:c++latest /EHsc /GL /Ox /Oi /W3 /MP /Zf /MD /Qpar /Qpar-report:1 /arch:AVX2 /nologo#/translateInclude
	/DNOMINMAX /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
)
target_include_directories(
	vulkan_module PUBLIC

	${include}/vulkan
)
# Add source to this project's executable.
add_executable(
	${PROJECT_NAME} 

	${source}/lighthouse/window.cpp
	${source}/lighthouse/file_system.cpp
	${source}/lighthouse/operating_system/dynamic_linking.cpp
	${source}/lighthouse/engine.cpp
	${source}/lighthouse/renderer/renderer.cpp
	${source}/lighthouse/version.cpp
	${source}/lighthouse/node.cpp
	${source}/lighthouse/operating_system/memory.cpp
	${source}/lighthouse/renderer/vulkan/extension.cpp
	${source}/lighthouse/renderer/vulkan/instance.cpp
	${source}/lighthouse/renderer/vulkan/debug_messanger.cpp
	${source}/lighthouse/renderer/vulkan/physical_device.cpp
	${source}/lighthouse/renderer/vulkan/logical_device.cpp
	${source}/lighthouse/renderer/vulkan/surface.cpp
	${source}/lighthouse/renderer/vulkan/queue_families.cpp
	${source}/lighthouse/renderer/vulkan/memory_allocator.cpp
	${source}/lighthouse/renderer/vulkan/command_control.cpp
	${source}/lighthouse/renderer/vulkan/queue.cpp
	${source}/lighthouse/renderer/vulkan/image.cpp
	${source}/lighthouse/renderer/vulkan/swapchain.cpp
	${source}/lighthouse/renderer/vulkan/descriptor_set_layout.cpp
	${source}/lighthouse/renderer/vulkan/buffer.cpp
	${source}/lighthouse/renderer/vulkan/sampler.cpp
	${source}/lighthouse/renderer/vulkan/descriptor_buffer.cpp
	${source}/lighthouse/renderer/vulkan/spir_v.cpp
	${source}/lighthouse/renderer/vulkan/shader_object.cpp
	${source}/lighthouse/renderer/vulkan/vertex_buffer.cpp
	${source}/lighthouse/renderer/vulkan/shader_object_pipeline.cpp
	${source}/lighthouse/renderer/scene_loader.cpp
	${source}/lighthouse/renderer/vulkan/pipeline_resource_generator.cpp
	${source}/lighthouse/renderer/vulkan/shader_input.cpp
	${source}/lighthouse/renderer/mesh.cpp
	${source}/lighthouse/renderer/bounding_volume.cpp
	${source}/lighthouse/output.cpp
	${source}/lighthouse/input.cpp
	${source}/vulkan/utils.cpp
	${source}/vulkan/math.cpp
	${source}/lighthouse/lightHouse.cpp
	${source}/lighthouse/entity.cpp
	${source}/lighthouse/camera.cpp
)

target_sources(
	${PROJECT_NAME} PRIVATE FILE_SET CXX_MODULES FILES

	${include}/lighthouse/window.ixx										
	${include}/lighthouse/static_type.ixx
	${include}/lighthouse/file_system.ixx									
	${include}/lighthouse/file_type.ixx
	${include}/lighthouse/utility.ixx
	${include}/lighthouse/datatype.ixx
	${include}/lighthouse/string/string.ixx
	${include}/lighthouse/string/string_convertible.ixx
	${include}/lighthouse/operating_system/system.ixx
	${include}/lighthouse/output.ixx
	${include}/lighthouse/input.ixx
	${include}/lighthouse/operating_system/dynamic_linking.ixx				
	${include}/lighthouse/renderer/vulkan/raii_wrapper.ixx
	${include}/lighthouse/renderer/vulkan/vulkan_string.ixx
	${include}/lighthouse/renderer/vulkan/vertex_format.ixx
	${include}/lighthouse/renderer/vulkan/vertex_input_description.ixx
	${include}/lighthouse/renderer/vulkan/utility.ixx
	${include}/lighthouse/renderer/vulkan/index_format.ixx
	${include}/lighthouse/engine.ixx										
	${include}/lighthouse/renderer/renderer.ixx								
	${include}/lighthouse/version.ixx										
	${include}/lighthouse/node.ixx											
	#${source}/vulkan/shaders.cpp
	${include}/lighthouse/operating_system/memory.ixx						
	${include}/lighthouse/renderer/vulkan/extension.ixx						
	${include}/lighthouse/renderer/vulkan/instance.ixx						
	${include}/lighthouse/renderer/vulkan/debug_messanger.ixx				
	${include}/lighthouse/renderer/vulkan/physical_device.ixx				
	${include}/lighthouse/renderer/vulkan/logical_device.ixx				
	${include}/lighthouse/renderer/vulkan/surface.ixx						
	${include}/lighthouse/renderer/vulkan/queue_families.ixx				
	${include}/lighthouse/renderer/vulkan/memory_allocator.ixx				
	${include}/lighthouse/renderer/vulkan/command_control.ixx				
	${include}/lighthouse/renderer/vulkan/queue.ixx							
	${include}/lighthouse/renderer/vulkan/image.ixx							
	${include}/lighthouse/renderer/vulkan/swapchain.ixx			
	${include}/lighthouse/renderer/vulkan/descriptor_set_layout.ixx			
	${include}/lighthouse/renderer/vulkan/buffer.ixx						
	${include}/lighthouse/renderer/vulkan/sampler.ixx						
	${include}/lighthouse/renderer/vulkan/descriptor_buffer.ixx				
	${include}/lighthouse/renderer/vulkan/spir_v.ixx						
	${include}/lighthouse/renderer/vulkan/shader_object.ixx					
	${include}/lighthouse/renderer/vulkan/vertex_buffer.ixx					
	${include}/lighthouse/renderer/vulkan/shader_object_pipeline.ixx		
	${include}/lighthouse/renderer/scene_loader.ixx							
	${include}/lighthouse/renderer/vulkan/pipeline_resource_generator.ixx	
	${include}/lighthouse/renderer/vulkan/shader_input.ixx					
	${include}/lighthouse/renderer/mesh.ixx									
	${include}/lighthouse/renderer/bounding_volume.ixx	
	${include}/lighthouse/entity.ixx
	${include}/lighthouse/camera.ixx
)

#STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#STRING (REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

# Compiler options
if(${CMAKE_CXX_COMPILER_ID} EQUAL MSVC)

  target_compile_options(
	  ${PROJECT_NAME} PRIVATE
	  # MSVC compiler settings
	  /std:c++latest /EHsc /GL /Ox /Oi /W3 /MP /Zf /MD /Qpar /Qpar-report:1 /arch:AVX2 /nologo /experimental:module#/translateInclude
	  # MSVC disable common annoying warnings
	  /wd4005 /wd4068 /wd4244 /wd4267 /wd4800
	  # Windows.h configuration
	  /DNOMINMAX /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
	  #/FIC:/Users/Stefan/source/repos/lighthouse/out/build/MSVC-x64-release/CMakeFiles/lighthouse.dir/RelWithDebInfo/cmake_pch.hxx
	)
	
	target_link_options(
		${PROJECT_NAME} PRIVATE
		# MSVC linker settings
		/INCREMENTAL /CGTHREADS:8 /VERBOSE:INCR /LTCG#/${library}/std/std.obj
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
	GLM_FORCE_DEPTH_ZERO_TO_ONE
	GLM_ENABLE_EXPERIMENTAL
	GLM_CONSTEXPR_SIMD
	GLM_FORCE_INTRINSICS

	# project definitions
	DYNAMIC_LINK_DIRECTORY="${dynamic}"
	INTELLISENSE=0#__INTELLISENSE__
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
	${PROJECT_NAME} PUBLIC

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

	std_module
	std_compat_module
	vulkan_module
)

message(cmake_version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})