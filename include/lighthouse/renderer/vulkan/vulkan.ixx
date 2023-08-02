module;
#pragma once

export module lh_vulkan;

#if INTELLISENSE
#include "lighthouse/renderer/vulkan/extension.ixx"
#include "lighthouse/renderer/vulkan/instance.ixx"
#include "lighthouse/renderer/vulkan/physical_device.ixx"
#include "lighthouse/renderer/vulkan/logical_device.ixx"
#include "lighthouse/renderer/vulkan/surface.ixx"
#include "lighthouse/renderer/vulkan/queue_families.ixx"
#include "lighthouse/renderer/vulkan/memory_allocator.ixx"
#include "lighthouse/renderer/vulkan/command_control.ixx"
#include "lighthouse/renderer/vulkan/queue.ixx"
#include "lighthouse/renderer/vulkan/image.ixx"
#include "lighthouse/renderer/vulkan/swapchain.ixx"
#include "lighthouse/renderer/vulkan/spir_v.ixx"
#include "lighthouse/renderer/vulkan/shader_module.ixx"
#include "lighthouse/renderer/vulkan/shader_object.ixx"
#include "lighthouse/renderer/vulkan/descriptor_set_layout.ixx"
#include "lighthouse/renderer/vulkan/descriptor_buffer.ixx"
#include "lighthouse/renderer/vulkan/pipeline_resource_generator.ixx"
#include "lighthouse/renderer/vulkan/shader_object_pipeline.ixx"
#include "lighthouse/renderer/vulkan/utility.ixx"
#include "lighthouse/renderer/vulkan/vertex_input_description.ixx"
#include "lighthouse/renderer/vulkan/vertex_buffer.ixx"
#include "lighthouse/renderer/vulkan/vertex_format.ixx"
#include "lighthouse/renderer/vulkan/index_format.ixx"
#include "lighthouse/renderer/vulkan/buffer.ixx"
#include "lighthouse/window.ixx"
#else
import window;
export import extension;
export import instance;
export import physical_device;
export import logical_device;
export import surface;
export import queue_families;
export import memory_allocator;
export import command_control;
export import queue;
export import image;
export import swapchain;
export import spir_v;
export import shader_module;
export import shader_object;
export import descriptor_set_layout;
export import descriptor_buffer;
export import pipeline_resource_generator;
export import shader_object_pipeline;
export import lh_utility;
export import vertex_input_description;
export import vertex_buffer;
export import vertex_format;
export import index_format;
export import buffer;
#endif
