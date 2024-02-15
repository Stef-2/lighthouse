module;

module user_interface;

namespace lh
{
	user_interface::user_interface(const window& window,
								   const vulkan::instance& instance,
								   const vulkan::physical_device& physical_device,
								   const vulkan::logical_device& logical_device,
								   const vulkan::queue_families::family& queue_family,
								   const vulkan::queue& queue,
								   const vulkan::swapchain& swapchain,
								   const create_info& create_info)
		: m_dear_imgui {window,
						instance,
						physical_device,
						logical_device,
						queue_family,
						queue,
						swapchain,
						create_info.m_dear_imgui_create_info}
	{}

	auto user_interface::dear_imgui() -> lh::dear_imgui&
	{
		return m_dear_imgui;
	}
}
