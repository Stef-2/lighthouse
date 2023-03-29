#pragma once

#include "lighthouse/string/string.hpp"
#include "lighthouse/vulkan/extension.hpp"
#include "lighthouse/vulkan/raii_wrapper.hpp"

namespace lh
{
	namespace vulkan
	{
		// forward declarations
		class instance;

		class physical_device : public vk_wrapper<vk::raii::PhysicalDevice>
		{
		public:
			using performance_score_t = uint64_t;

			struct create_info
			{
				physical_extensions::create_info m_extensions = physical_extensions::m_default_physical_extensions;
				performance_score_t m_minimum_accepted_score {0xFFFFFFFF};
			};

			physical_device(const instance&, const create_info& = {});

			auto extensions() const -> physical_extensions;
			auto performance_score() const -> performance_score_t;
			auto info() const -> lh::string::string_t override;

		private:
			static auto performance_score(const vk::raii::PhysicalDevice&) -> performance_score_t;
			static auto preferred_device(const instance&, const create_info&) -> vk::raii::PhysicalDevice;

			physical_extensions m_extensions;
			performance_score_t m_performance_score;
		};
	}
}
