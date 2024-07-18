module;

#if INTELLISENSE
#include "vulkan/vulkan.hpp"
#endif

export module extension;

import lighthouse_string;
import vulkan_string;
import output;

#if not INTELLISENSE
import vulkan_hpp;
#endif

import std;

export namespace lh
{
	namespace vulkan
	{
		using vk_layers_t = const std::vector<vk::LayerProperties>;
		using vk_extensions_t = const std::vector<vk::ExtensionProperties>;
		
		// ==========================================================================
		// abstract base for vulkan logical, phyisical and validation extensions
		// ==========================================================================
		template <typename T>
			requires std::is_same_v<T, vk_layers_t> or std::is_same_v<T, vk_extensions_t>
		class vulkan_extension_module
		{
		public:
			struct create_info
			{
				vk_string_t m_required_extensions {};
			};

			vulkan_extension_module(const T& supported, const create_info& create_info)
				: m_supported_extensions(supported), m_required_extensions(create_info.m_required_extensions)
			{
				if (not assert_required_extensions())
					::lh::output::error() << "this system does not support the required " + extension_type() + "s";
			}

			auto required_extensions() const -> vk_string_t { return m_required_extensions; }
			auto supported_extensions() const -> T { return m_supported_extensions; }
			auto assert_required_extensions() const -> bool
			{
				const auto supported = supported_extensions();

				for (const auto& required : required_extensions())
				{
					if (std::find_if(supported.begin(), supported.end(), [&required, this](const auto& supported) {
							return std::strcmp(required, extension_name(supported).c_str()) == 0;
						}) == supported.end())
					{
						output::error() << "this system does not support the required " + extension_type() + ": " +
											   lh::string::string_t {required};
						return false;
					}
				}
				return true;
			}

			auto info() const -> lh::string::string_t
			{
				const auto supported_extensions = vulkan_extension_module::supported_extensions();
				const auto required_extensions = vulkan_extension_module::required_extensions();

				const auto supported =
					std::accumulate(supported_extensions.begin(),
									supported_extensions.end(),
									lh::string::string_t {"\n======== supported " + extension_type() + ": ========\n"},
									[this](const auto& accumulator, const auto& extension) {
										return std::move(accumulator) + '\t' + extension_name(extension) + '\n';
									});

				const auto required = std::accumulate(required_extensions.begin(),
													  required_extensions.end(),
													  lh::string::string_t {"\n======== required " + extension_type() +
																			": ========\n"},
													  [](const auto& accumulator, const auto& extension) {
														  return std::move(accumulator) + '\t' +
																 static_cast<const char*>(extension) + '\n';
													  });

				return supported + required;
			}

		private:
			typedef T::value_type extension_t;

			virtual auto extension_type() const -> lh::string::string_t { return {}; };
			auto extension_name(const extension_t& extension) const -> lh::string::string_t
			{
				if constexpr (std::is_same_v<T, vk_layers_t>)
					return extension.layerName.data();
				else
					return extension.extensionName.data();
			}

			T m_supported_extensions;
			vk_string_t m_required_extensions;
		};

		// ==========================================================================
		// vulkan extension specialization for logical (instance) extensions
		// ==========================================================================
		class logical_extensions : public vulkan_extension_module<vk_extensions_t>
		{
		public:
			using vulkan_extension_module::vulkan_extension_module;

			logical_extensions(const vk_extensions_t& supported, const create_info& create_info = {});

		private:
			auto extension_type() const -> lh::string::string_t override;
		};

		// ==========================================================================
		// vulkan extension specialization for physical (device) extensions
		// ==========================================================================
		class physical_extensions : public vulkan_extension_module<vk_extensions_t>
		{
		public:
			using vulkan_extension_module::vulkan_extension_module;

			physical_extensions(const vk_extensions_t& supported, const create_info& create_info = {});

		private:
			auto extension_type() const -> lh::string::string_t override;
		};

		// ==========================================================================
		// vulkan extension specialization for validation layers
		// ==========================================================================
		class validation_layers : public vulkan_extension_module<vk_layers_t>
		{
		public:
			validation_layers(const vk_layers_t& supported, const create_info& create_info = {});

		private:
			auto extension_type() const -> lh::string::string_t override;
		};
	}
}