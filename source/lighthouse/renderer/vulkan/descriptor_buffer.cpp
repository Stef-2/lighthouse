module;

module descriptor_buffer;
import output;

namespace lh
{
	namespace vulkan
	{

		descriptor_buffer::descriptor_buffer()
			: m_descriptor_buffer {}, m_binding_info {}, m_bind_point {vk::PipelineBindPoint::eGraphics}
		{}

		descriptor_buffer::descriptor_buffer(const physical_device& physical_device,
											 const logical_device& logical_device,
											 const memory_allocator& memory_allocator,
											 const descriptor_set_layout& descriptor_set_layout,
											 const buffer_subdata& data,
											 const create_info& create_info)
			: m_descriptor_buffer {}, m_binding_info {}, m_bind_point {create_info.m_bind_point}
		{
			const auto binding_count = descriptor_set_layout.bindings().size();

			const auto assert_binding_and_data_matching = binding_count == data.m_subdata.size();
			if (not assert_binding_and_data_matching)
				lh::output::error() << "descriptor set layout bindings and provided buffer data do not match";

			const auto descriptor_buffer_properties = physical_device.properties().m_descriptor_buffer_properties;
			m_binding_info.reserve(binding_count);

			m_descriptor_buffer = mapped_buffer {
				logical_device,
				memory_allocator,
				descriptor_set_layout->getSizeEXT() * binding_count,
				mapped_buffer::create_info {.m_usage = descriptor_buffer_usage(descriptor_set_layout),
											.m_properties = create_info.descriptor_collection_memory_properties}};

			for (std::size_t i {}; i < binding_count; i++)
			{
				const auto& binding = descriptor_set_layout.bindings()[i];
				const auto binding_offset = descriptor_set_layout->getBindingOffsetEXT(binding.m_binding);

				m_binding_info.emplace_back(
					m_descriptor_buffer.address() +
						i * utility::aligned_size(binding_offset,
												  descriptor_buffer_properties.descriptorBufferOffsetAlignment),
					descriptor_buffer_usage(descriptor_set_layout));

				const auto data_address_info = vk::DescriptorAddressInfoEXT {data.m_buffer->address() +
																				 data.m_subdata[i].m_offset,
																			 data.m_subdata[i].m_size};

				const auto& descriptor_info = static_cast<VkDescriptorGetInfoEXT>(
					vk::DescriptorGetInfoEXT {binding.m_type, {&data_address_info}});

				logical_device->getDispatcher()->vkGetDescriptorEXT(
					**logical_device,
					&descriptor_info,
					descriptor_buffer::descriptor_size(physical_device, binding.m_type),
					static_cast<std::byte*>(m_descriptor_buffer.allocation_info().pMappedData) + binding_offset);
			}
		}

		auto descriptor_buffer::buffer() -> const mapped_buffer&
		{
			return m_descriptor_buffer;
		}

		auto descriptor_buffer::bind(const vk::raii::CommandBuffer& command_buffer,
									 const vk::raii::PipelineLayout& pipeline_layout) const -> void
		{
			command_buffer.bindDescriptorBuffersEXT(m_binding_info);
			command_buffer.setDescriptorBufferOffsetsEXT(m_bind_point, *pipeline_layout, 0, {0}, {0});
		}

		auto descriptor_buffer::descriptor_size(const physical_device& physical_device,
												const vk::DescriptorType& descriptor_type) -> const std::size_t
		{
			const auto& descriptor_properties = physical_device.properties().m_descriptor_buffer_properties;

			switch (descriptor_type)
			{
				case vk::DescriptorType::eUniformBuffer: return descriptor_properties.uniformBufferDescriptorSize;
				case vk::DescriptorType::eCombinedImageSampler:
					return descriptor_properties.combinedImageSamplerDescriptorSize;
				case vk::DescriptorType::eStorageBuffer: return descriptor_properties.storageBufferDescriptorSize;
				default: break;
			}

			std::unreachable();
		}

		auto descriptor_buffer::descriptor_buffer_usage(const descriptor_set_layout& descriptor_set_layout)
			-> const vk::BufferUsageFlags
		{
			auto usage = vk::BufferUsageFlags {vk::BufferUsageFlagBits::eShaderDeviceAddress};

			for (const auto& binding : descriptor_set_layout.bindings())
			{
				if (binding.m_type == vk::DescriptorType::eStorageBuffer or
					binding.m_type == vk::DescriptorType::eStorageBufferDynamic or
					binding.m_type == vk::DescriptorType::eUniformBuffer or
					binding.m_type == vk::DescriptorType::eUniformBufferDynamic)

					usage = usage | vk::BufferUsageFlagBits::eResourceDescriptorBufferEXT;

				if (binding.m_type == vk::DescriptorType::eCombinedImageSampler or
					binding.m_type == vk::DescriptorType::eSampler)

					usage = usage | vk::BufferUsageFlagBits::eSamplerDescriptorBufferEXT;
			}

			return usage;
		}
	}
}
