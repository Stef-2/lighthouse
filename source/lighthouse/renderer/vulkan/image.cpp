module image;

namespace lh
{
	namespace vulkan
	{
		image::image(std::nullptr_t) : m_create_info {}, /*m_view {nullptr},*/ m_allocation_info {}, m_allocation {} {}

		image::image(const vulkan::logical_device& logical_device,
					 const vulkan::memory_allocator& memory_allocator,
					 const create_info& create_info)
			: m_create_info {create_info}, /*m_view {nullptr},*/ m_allocation_info {}, m_allocation {}
		{
			auto allocation_info = vma::AllocationInfo {};

			auto [image, allocation] = memory_allocator->createImage(m_create_info.m_image_create_info,
																	 m_create_info.m_allocation_create_info,
																	 allocation_info);

			m_object = {*logical_device, image};
			vk::ImageViewCreateInfo
			m_create_info.m_image_view_create_info.setImage(*m_object);
			//m_view = {*logical_device, m_create_info.m_image_view_create_info};
		}

		auto image::create_information() const -> const image::create_info&
		{
			return m_create_info;
		}
		/*
		auto image::view() const -> const vk::raii::ImageView&
		{
			return m_view;
		}*/

		auto image::transition_layout(const vk::raii::CommandBuffer& command_buffer,
									  const layout_transition_data& transition_data) -> void
		{
			const auto barrier = vk::ImageMemoryBarrier2 {transition_data.m_source_pipeline_stage,
														  transition_data.m_source_access_flags,
														  transition_data.m_destination_pipeline_stage,
														  transition_data.m_destination_access_flags,
														  transition_data.m_old_layout,
														  transition_data.m_new_layout,
														  transition_data.m_source_queue_family,
														  transition_data.m_destination_queue_family,
														  *m_object,
														  transition_data.m_subresource_range};

			const auto dependency_info = vk::DependencyInfo {{}, {}, {}, barrier};

			command_buffer.pipelineBarrier2(dependency_info);
		}

		auto image::allocation_info() const -> const vma::AllocationInfo&
		{
			return m_allocation_info;
		}
		auto image::allocation_info() -> vma::AllocationInfo&
		{
			return m_allocation_info;
		}
		auto image::allocation() const -> const vma::Allocation&
		{
			return m_allocation;
		}
	}
}
