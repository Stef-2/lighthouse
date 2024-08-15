module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#include "vulkan/vma/vk_mem_alloc.hpp"
#endif

export module buffer;

import lighthouse_utility;
import memory_mapped_span;
import data_type;
import raii_wrapper;
import logical_device;
import memory_block;
import memory_allocator;
import virtual_allocator;
import output;
import queue;

import std;

export namespace lh
{
	namespace vulkan
	{
		class mapped_buffer;

		class buffer : public raii_wrapper<vk::raii::Buffer>
		{
		public:
			using raii_wrapper::raii_wrapper;

			using used_memory_percentage_t = float32_t;

			struct create_info
			{
				vk::BufferUsageFlags m_usage = {};
				vk::MemoryPropertyFlags m_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
				vma::AllocationCreateInfo m_allocation_create_info = {{},
																	  vma::MemoryUsage::eAuto,
																	  m_memory_properties,
																	  m_memory_properties};
			};

			static inline constexpr auto s_create_info = buffer::create_info {
				.m_usage = {},
				.m_memory_properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
				.m_allocation_create_info = {{},
																  vma::MemoryUsage::eAuto,
																  vk::MemoryPropertyFlagBits::eDeviceLocal,
																  vk::MemoryPropertyFlagBits::eDeviceLocal}};

			buffer(const logical_device&, const memory_allocator&, const vk::DeviceSize, const create_info& = buffer::s_create_info);
			buffer(buffer&&) noexcept;
			buffer& operator=(buffer&&) noexcept;
			~buffer();

			auto allocation_info() const -> const vma::AllocationInfo&;
			auto allocation_info() -> vma::AllocationInfo&;
			auto allocation() const -> const vma::Allocation&;

			auto address() const -> const vk::DeviceAddress&;
			auto size() const -> const vk::DeviceSize&;
			//auto used_memory() const -> const vk::DeviceSize&;
			//auto remaining_memory() const -> const vk::DeviceSize;
			//auto used_memory_percentage() const -> const used_memory_percentage_t;
			auto create_information() const -> const create_info&;
			
			template <typename T>
				requires(not std::is_pointer_v<T>)
			auto upload_data(queue& queue,
							 const T& data,
							 const std::size_t& offset = 0,
							 const std::size_t& size = sizeof(T));

		protected:
			const logical_device* m_logical_device;
			const memory_allocator* m_allocator;
			vma::AllocationInfo m_allocation_info;
			vma::Allocation m_allocation;

			vk::DeviceAddress m_address;
			//vk::DeviceSize m_used_memory;

			create_info m_create_info;
		};

		// =========================================================================

		class mapped_buffer : public buffer
		{
		public:
			using buffer::buffer;
			
			static inline constexpr auto s_create_info = buffer::create_info
			{
				.m_usage = {vk::BufferUsageFlagBits::eShaderDeviceAddress},
				.m_memory_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
														vk::MemoryPropertyFlagBits::eHostCoherent},
				.m_allocation_create_info = {vma::AllocationCreateFlagBits::eMapped,
																	  vma::MemoryUsage::eAuto,
																  {vk::MemoryPropertyFlagBits::eHostVisible |
																   vk::MemoryPropertyFlagBits::eHostCoherent},
					{vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent}}
			};

			mapped_buffer(const logical_device&,
						  const memory_allocator&,
						  const vk::DeviceSize,
						  const create_info& = mapped_buffer::s_create_info);
			~mapped_buffer();
			mapped_buffer(mapped_buffer&&) noexcept;
			mapped_buffer& operator=(mapped_buffer&&) noexcept;

			template <typename T>
				requires(not std::is_pointer_v<T>)
			auto upload_data(queue&,
							 const T&,
							 const std::size_t&,
							 const std::size_t&) = delete;

			auto mapped_data_pointer() const -> void*;
			auto map() -> void;
			auto unmap() -> void;

			template <typename T>
			requires (not std::is_pointer_v<T>)
			auto map_data(const T& data, const std::size_t& offset = 0, const std::size_t& size = sizeof(T)) const;

		protected:
			void* m_mapped_data_pointer;
		};

		// ===========================================================================

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

		private:
		};

		// ===========================================================================

		template <typename T = mapped_buffer>
			requires lh::concepts::is_any<T, buffer, mapped_buffer>
		class suballocated_buffer : public T
		{
		public:
			suballocated_buffer(const logical_device& logical_device,
									   const memory_allocator& memory_allocator,
									   const vk::DeviceSize size,
									   const T::create_info& create_info = T::s_create_info)
				: T {logical_device, memory_allocator, size, create_info},
				  m_virtual_allocator {size}
			{}

			suballocated_buffer(const suballocated_buffer&) = delete;
			auto operator=(const suballocated_buffer&) -> suballocated_buffer& = delete;

			template <typename Y>
				requires std::is_same_v<T, mapped_buffer>
			[[nodiscard]] auto request_and_commit_span(std::size_t element_count) -> memory_mapped_span<Y>
			{
				const auto memory_offset = m_virtual_allocator.request_and_commit_suballocation(element_count * sizeof (Y));

				if (memory_offset == std::numeric_limits<virtual_allocator::memory_offset_t>::max())
					output::error() << lh::string::string_t {"could not allocate: " + element_count * sizeof (Y)}.append(
						" bytes from buffer at address: " + T::address());

				const auto memory_address = static_cast<std::byte*>(T::m_mapped_data_pointer) + memory_offset;

				return memory_mapped_span<Y> {reinterpret_cast<Y*>(memory_address), element_count};
			}

			template <typename Y>
				requires std::is_same_v<T, mapped_buffer>
			auto free_span(const memory_mapped_span<Y>& span) -> void
			{
				m_virtual_allocator.free_suballocation(reinterpret_cast<std::uintptr_t>(span.data()) - static_cast<virtual_allocator::memory_offset_t>(Y::m_mapped_data_pointer));
			}

			template <typename Y>
				requires std::is_same_v<T, mapped_buffer>
			auto span_device_address(const memory_mapped_span<Y>& span) -> const vk::DeviceAddress
			{
				return mapped_buffer::address() + reinterpret_cast<std::uintptr_t>(T::m_mapped_data_pointer) -
					   reinterpret_cast<std::uintptr_t>(span.data());
			}

			template <typename Y>
				requires std::is_same_v<T, buffer>
			auto request_and_commit_range(std::size_t range_size)-> const lh::memory_block
			{
				const auto memory_offset = m_virtual_allocator.request_and_commit_suballocation(range_size);

				if (memory_offset == std::numeric_limits<virtual_allocator::memory_offset_t>::max())
					output::error() << lh::string::string_t {"could not allocate: " + range_size}.append(
						" bytes from buffer at address: " + T::address());

				return {memory_offset, range_size};
			}

			template <typename Y>
				requires std::is_same_v<T, buffer>
			auto free_range(const lh::memory_block& memory_block) -> void
			{
				m_virtual_allocator.free_suballocation(memory_block.m_offset);
			}

			template <typename Y>
				requires std::is_same_v<T, buffer>
			auto range_address(const memory_block& memory_block) -> const vk::DeviceAddress
			{
				return mapped_buffer::address() + memory_block.m_offset;
			}

		private:
			lh::virtual_allocator m_virtual_allocator;
		};

		// =========================================================================

		template <typename T = mapped_buffer, std::size_t N = std::dynamic_extent>
			requires lh::concepts::is_any<T, buffer, mapped_buffer>
		struct buffer_subdata
		{
			struct subdata
			{
				vk::DeviceAddress m_offset;
				vk::DeviceSize m_size;
			};

			using subdata_storage_t = std::conditional<N == std::dynamic_extent, std::vector<subdata>, std::array<subdata, N>>::type;

			auto operator[](std::size_t index) const -> const subdata& { return m_subdata[index]; }

			lh::non_owning_ptr<T> m_buffer;
			/*std::vector<subdata>*/subdata_storage_t m_subdata;
		};

		template <typename T>
			requires(not std::is_pointer_v<T>)
		auto buffer::upload_data(queue& queue, const T& data, const std::size_t& offset, const std::size_t& size)
		{
			const auto staging_buffer = mapped_buffer {
				*m_logical_device,
				*m_allocator,
				size,
				{.m_usage = vk::BufferUsageFlagBits::eTransferSrc,
				 .m_memory_properties = {vk::MemoryPropertyFlagBits::eHostVisible |
										 vk::MemoryPropertyFlagBits::eHostCoherent},
				 .m_allocation_create_info = {
					 vma::AllocationCreateFlagBits::eMapped,
					 vma::MemoryUsage::eAuto,
					 {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},
					 {vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent}}}};
			staging_buffer.map_data(data, 0, size);

			queue.command_control().reset();
			const auto& command_buffer = queue.command_control().front();

			command_buffer.begin(queue.command_control().usage_flags());
			const auto buffer_copy = vk::BufferCopy2 {0, offset, size};
			command_buffer.copyBuffer2(vk::CopyBufferInfo2 {*staging_buffer, *m_object, {buffer_copy}});
			command_buffer.end();

			queue.submit_and_wait();
			// m_used_memory = std::max(m_used_memory, offset + size);
		}

		template <typename T>
			requires(not std::is_pointer_v<T>)
		inline auto mapped_buffer::map_data(const T& data, const std::size_t& offset, const std::size_t& size) const
		{
			const auto destination = static_cast<std::byte*>(m_mapped_data_pointer) + offset;

			std::memcpy(destination, &data, size);
		}
	}
}