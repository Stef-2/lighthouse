module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"

#include <vector>
#include <cstddef>
#include <cstring>
#endif

export module buffer;

import lighthouse_utility;
import memory_mapped_span;
import raii_wrapper;
import logical_device;
import memory_allocator;
import queue;

export namespace lh
{
	namespace vulkan
	{
		class mapped_buffer;

		class buffer : public raii_wrapper<vk::raii::Buffer>
		{
		public:
			using raii_wrapper::raii_wrapper;

			using used_memory_percentage_t = float;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vk::MemoryPropertyFlags m_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
				vma::AllocationCreateInfo m_allocation_create_info = {{},
																	  vma::MemoryUsage::eAuto,
																	  m_memory_properties,
																	  m_memory_properties};
			};

			buffer(const logical_device&, const memory_allocator&, const vk::DeviceSize, const create_info& = {});
			buffer(buffer&&) noexcept = default;
			buffer& operator=(buffer&&) noexcept = default;
			~buffer();

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

			auto address() const -> const vk::DeviceAddress&;
			auto size() const -> const vk::DeviceSize&;
			auto used_memory() const -> const vk::DeviceSize&;
			auto remaining_memory() const -> const vk::DeviceSize;
			auto used_memory_percentage() const -> const used_memory_percentage_t;
			auto create_information() const -> const create_info&;
			
			template <typename T>
			requires(not std::is_pointer_v<T>)
			auto upload_data(queue& queue,
							 const T& data,
							 const std::size_t& offset = 0,
							 const std::size_t& size = sizeof(T))
			{
				const auto staging_buffer = mapped_buffer {*m_logical_device, *m_allocator, size};
				staging_buffer.map_data(data);

				const auto& command_buffer = queue.command_control().front();
				command_buffer.begin(queue.command_control().usage_flags());
				command_buffer.copyBuffer(*staging_buffer, *m_object, {0, offset, size});

				command_buffer.end();

				queue.submit_and_wait();
			}

		protected:
			const logical_device* m_logical_device;
			const memory_allocator* m_allocator;
			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;

			vk::DeviceAddress m_address;
			vk::DeviceSize m_used_memory;

			create_info m_create_info;
		};

		class mapped_buffer : public buffer
		{
		public:
			using buffer::buffer;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {vk::BufferUsageFlagBits::eShaderDeviceAddress};
				vk::MemoryPropertyFlags m_memory_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
														vk::MemoryPropertyFlagBits::eHostCoherent};
				vma::AllocationCreateInfo m_allocation_create_info = {vma::AllocationCreateFlagBits::eMapped,
																	  vma::MemoryUsage::eAuto,
																	  m_memory_properties,
																	  m_memory_properties};
			};

			mapped_buffer(const logical_device&,
						  const memory_allocator&,
						  const vk::DeviceSize,
						  const mapped_buffer::create_info& = {});
			~mapped_buffer();
			mapped_buffer(mapped_buffer&&) = default;
			mapped_buffer& operator=(mapped_buffer&&) = default;

			auto mapped_data_pointer() const -> void*;
			auto map() -> void;
			auto unmap() -> void;

			template <typename T>
			requires (not std::is_pointer_v<T>)
			auto map_data(const T& data, const std::size_t& offset = 0, const std::size_t& size = sizeof(T)) const
			{
				const auto destination = static_cast<std::byte*>(m_mapped_data_pointer) + offset;
				
				std::memcpy(destination, &data, size);
			}

		protected:
			void* m_mapped_data_pointer;
		};

		template <typename T>
		class mapped_buffer_span : public mapped_buffer, public memory_mapped_span<T>
		{
		public:
			mapped_buffer_span(const logical_device& logical_device,
							   const memory_allocator& memory_allocator,
							   const std::size_t element_count,
							   const mapped_buffer::create_info& create_info = {})
				: mapped_buffer {logical_device, memory_allocator, element_count * sizeof T, create_info},
				  memory_mapped_span<T> {static_cast<T*>(this->m_mapped_data_pointer), m_allocation_info.size / sizeof T}
			{}
			//mapped_buffer_span(mapped_buffer&&);

			//auto buffer() -> mapped_buffer&;
			//auto buffer() const -> const mapped_buffer&;
			//auto span() -> memory_mapped_span<T>&;
			//auto span() const -> const memory_mapped_span<T>&;

		private:
		};

		struct buffer_subdata
		{
			struct subdata
			{
				vk::DeviceAddress m_offset;
				vk::DeviceSize m_size;
			};

			auto operator[](std::size_t index) const -> const subdata&;

			lh::non_owning_ptr<mapped_buffer> m_buffer;
			std::vector<subdata> m_subdata;
		};
	}
}