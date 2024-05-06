module;

export module descriptor_layout;

export namespace lh
{
	namespace vulkan
	{
		constexpr auto g_set_count = 3;

		enum class descriptor_index
		{
			uniform_buffer_set = 0,
			storage_buffer_set = 1,
			combined_image_sampler_set = 2
		};
	}
}