module;

#if INTELLISENSE
#include "vulkan/vulkan_raii.hpp"
#endif

export module queue;

import raii_wrapper;
import logical_device;
import queue_families;
import command_control;
import swapchain;
import buffer;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		class queue : public raii_wrapper<vk::raii::Queue>
		{
		public:
			using fence_timeout_t = std::uint64_t;

			struct create_info
			{
				queue_families::family m_queue_family;
				fence_timeout_t m_fence_timeout = std::numeric_limits<fence_timeout_t>::max();
				command_control::create_info m_command_control_create_info {};
			};
			
			struct semaphore
			{
				vk::Semaphore m_semaphore = {};
				std::uint64_t m_semaphore_value = 0;
				vk::PipelineStageFlags2 m_pipeline_stage = {};
			};

			enum class queue_state
			{
				initial,
				recording,
				executing
			};

			queue(const logical_device&, const create_info& = {});

			auto add_submit_wait_semaphore(const semaphore&) -> void;
			auto add_submit_signal_semaphore(const semaphore&) -> void;
			auto submit() -> void;
			auto submit_and_wait() -> void;

			auto command_control() const -> const vulkan::command_control&;
			auto record_commands() -> const vk::raii::CommandBuffer&;
			auto queue_state() const -> const queue::queue_state&;

		protected:
			virtual auto clear() -> void;

			const logical_device& m_logical_device;

			decltype(queue_state::initial) m_queue_state;
			vulkan::command_control m_command_control;

			fence_timeout_t m_fence_timeout;
			vk::raii::Fence m_submit_fence;

			std::vector<vk::SemaphoreSubmitInfo> m_submit_wait_semaphores;
			std::vector<vk::SemaphoreSubmitInfo> m_submit_signal_semaphores;
		};

		class graphics_queue : public queue
		{
		public:
			graphics_queue(const logical_device&, const swapchain&, const create_info& = {});

			auto add_present_wait_semaphore(const vk::Semaphore&) -> void;
			auto present() const -> void;
			auto present_and_wait() -> void;

		private:
			auto clear() -> void override final;

			const swapchain& m_swapchain;

			vk::raii::Fence m_present_fence;
			std::vector<vk::Semaphore> m_present_wait_semaphores;
		};
		
		class transfer_queue : public queue
		{
		public:

			template <typename T>
				requires(not std::is_pointer_v<T>)
			struct data_upload_info
			{
				const T& m_data;
				std::size_t m_offset = 0;
				std::size_t m_size = sizeof T;
			};

			transfer_queue(const logical_device&, suballocated_buffer<mapped_buffer>&, const create_info& = {});

			template <typename T>
				requires(not std::is_pointer_v<T>)
			auto record_data_upload(const buffer& buffer, const data_upload_info<T>& data_upload_info)
			{
				const auto buffer_copy = vk::BufferCopy2 {0, data_upload_info.m_offset, data_upload_info.m_size};
				m_recorded_copies.emplace_back(vk::CopyBufferInfo2 {vk::Buffer {}, *buffer, {buffer_copy}});
			}

			template <typename T>
				requires(not std::is_pointer_v<T>)
			auto upload_data_and_wait(const buffer& buffer, const data_upload_info<T>& data_upload_info)
			{
				auto span = m_suballocated_buffer.request_and_commit_span<T>(data_upload_info.m_size);
				std::memcpy(span.data(), &data_upload_info.m_data, data_upload_info.m_size);

				m_command_control.reset();
				const auto& command_buffer = m_command_control.front();

				command_buffer.begin(m_command_control.usage_flags());

				const auto buffer_copy = vk::BufferCopy2 {0, data_upload_info.m_offset, data_upload_info.m_size};
				command_buffer.copyBuffer2(vk::CopyBufferInfo2 {*m_suballocated_buffer, *buffer, {buffer_copy}});

				command_buffer.end();

				submit_and_wait();

				m_suballocated_buffer.free_span(span);
			}

		private:
			auto clear() -> void override final;

			suballocated_buffer<mapped_buffer>& m_suballocated_buffer;

			std::vector<vk::CopyBufferInfo2> m_recorded_copies;
		};
	}
}