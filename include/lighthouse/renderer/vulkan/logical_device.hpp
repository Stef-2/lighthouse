#pragma once

#include "lighthouse/renderer/vulkan/raii_wrapper.hpp"
#include "lighthouse/renderer/vulkan/string.hpp"
#include "lighthouse/string/string.hpp"

#include <vector>

namespace lh
{
	namespace vulkan
	{

		// forward declarations
		class physical_device;

		class logical_device : public vk_wrapper<vk::raii::Device>
		{
		public:
			struct create_info
			{
				decltype(*vk::DeviceQueueCreateInfo::pQueuePriorities) m_queue_priority {1.0f};
				std::vector<vk::DeviceQueueCreateInfo> m_queues {};

				vk::PhysicalDeviceFeatures2 m_features = vk::PhysicalDeviceFeatures2 {
					vk::PhysicalDeviceFeatures {// robustBufferAccess
												{false},
												// fullDrawIndexUint32
												{false},
												// imageCubeArray
												{false},
												// independentBlend
												{false},
												// geometryShader
												{false},
												// tessellationShader
												{false},
												// sampleRateShading
												{false},
												// dualSrcBlend
												{false},
												// logicOp
												{false},
												// multiDrawIndirect
												{false},
												// drawIndirectFirstInstance
												{false},
												// depthClamp
												{false},
												// depthBiasClamp
												{false},
												// fillModeNonSolid
												{false},
												// depthBounds
												{false},
												// wideLines
												{false},
												// largePoints
												{false},
												// alphaToOne
												{false},
												// multiViewport
												{false},
												// samplerAnisotropy
												{true},
												// textureCompressionETC2
												{false},
												// textureCompressionASTC_LDR
												{false},
												// textureCompressionBC
												{false},
												// occlusionQueryPrecise
												{false},
												// pipelineStatisticsQuery
												{false},
												// vertexPipelineStoresAndAtomics
												{false},
												// fragmentStoresAndAtomics
												{false},
												// shaderTessellationAndGeometryPointSize
												{false},
												// shaderImageGatherExtended
												{false},
												// shaderStorageImageExtendedFormats
												{false},
												// shaderStorageImageMultisample
												{false},
												// shaderStorageImageReadWithoutFormat
												{false},
												// shaderStorageImageWriteWithoutFormat
												{false},
												// shaderUniformBufferArrayDynamicIndexing
												{false},
												// shaderSampledImageArrayDynamicIndexing
												{false},
												// shaderStorageBufferArrayDynamicIndexing
												{false},
												// shaderStorageImageArrayDynamicIndexing
												{false},
												// shaderClipDistance
												{false},
												// shaderCullDistance
												{false},
												// shaderFloat64
												{false},
												// shaderInt64
												{false},
												// shaderInt16
												{false},
												// shaderResourceResidency
												{false},
												// shaderResourceMinLod
												{false},
												// sparseBinding
												{false},
												// sparseResidencyBuffer
												{false},
												// sparseResidencyImage2D
												{false},
												// sparseResidencyImage3D
												{false},
												// sparseResidency2Samples
												{false},
												// sparseResidency4Samples
												{false},
												// sparseResidency8Samples
												{false},
												// sparseResidency16Samples
												{false},
												// sparseResidencyAliased
												{false},
												// variableMultisampleRate
												{false},
												// inheritedQueries
												{false}}};
				vk_string_t m_extensions {};
			};

			logical_device(const physical_device&, const create_info&);

			auto info() const -> lh::string::string_t override;

		private:
		};
	}
}