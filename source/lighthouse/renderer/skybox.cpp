module;

module skybox;

namespace lh
{
	skybox::skybox(const vulkan::physical_device& physical_device,
				   const vulkan::logical_device& logical_device,
				   const vulkan::memory_allocator& memory_allocator,
				   const vulkan::global_descriptor& global_descriptor,
				   const vulkan::descriptor_buffer& descriptor_buffer,
				   const mesh_registry& mesh_registry,
				   const vulkan::shader_pipeline::pipeline_glsl_code_t& pipeline_code,
				   const skybox_texture_paths_t& texture_paths,
				   vulkan::queue& queue,
				   const create_info& create_info)
		: m_mesh {mesh_registry.cube()},
		  m_pipeline_resource_generator {
			  physical_device, logical_device, memory_allocator, pipeline_code, global_descriptor, descriptor_buffer},
		  m_texture {physical_device,
					 logical_device,
					 memory_allocator,
					 queue,
					 vulkan::texture::image_paths_t {texture_paths.begin(), texture_paths.end()},
					 descriptor_buffer,
					 {vulkan::image::create_info {.m_image_create_info = {vk::ImageCreateFlagBits::eCubeCompatible,
																		  vk::ImageType::e2D,
																		  vulkan::image::create_info::m_format,
																		  vk::Extent3D {},
																		  1,
																		  6,
																		  vk::SampleCountFlagBits::e1,
																		  vk::ImageTiling::eOptimal,
																		  vk::ImageUsageFlagBits::eSampled,
																		  vk::SharingMode::eExclusive}},
					  vulkan::image_view::create_info {
						  .m_create_info = {{},
											{},
											vk::ImageViewType::eCube,
											vulkan::image::create_info::m_format,
											{},
											{vk::ImageAspectFlagBits::eColor, 0, 1, 0, vk::RemainingArrayLayers}}},
					  vulkan::sampler::create_info {.m_create_info = {{},
																	  vk::Filter::eNearest,
																	  vk::Filter::eNearest,
																	  vk::SamplerMipmapMode::eNearest,
																	  vk::SamplerAddressMode::eClampToEdge,
																	  vk::SamplerAddressMode::eClampToEdge,
																	  vk::SamplerAddressMode::eClampToEdge,
																	  0.0f,
																	  false,
																	  0.0f,
																	  false,
																	  vk::CompareOp::eNever,
																	  0.0f,
																	  0.0f,
																	  vk::BorderColor::eFloatTransparentBlack,
																	  true}}}}

	{}

	auto skybox::mesh() const -> const lh::mesh&
	{
		return m_mesh;
	}

	auto skybox::pipeline() const -> const vulkan::pipeline&
	{
		return m_pipeline_resource_generator;
	}

	auto skybox::texture() const -> const vulkan::texture&
	{
		return m_texture;
	}
}
