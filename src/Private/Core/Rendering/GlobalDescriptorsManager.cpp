#include "Rendering/GlobalDescriptorsManager.hpp"
#include <Rendering/SwapChain.hpp>

namespace crsp {
	GlobalDescriptorsManager::GlobalDescriptorsManager(Device& device, VkDescriptorImageInfo shadowDescriptorImageInfo) : device{device}
	{
		// TODO: Add pool manager that creates new pools on demand.
		globalPool = DescriptorPool::Builder(device)
			.setMaxSets(2 * SwapChain::MAX_FRAMES_IN_FLIGHT + MAX_DECRIPTOR_SETS) // Global descriptor sets + any additional material sets
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT) // Global UBO buffer
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_UNIFORM_BUFFER_DESCRIPTORS) // Any additional uniform buffers
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1) // Shadow map
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_TEXTURE_DESCRIPTORS) // Any additional texture resources
			.build();

		// Create global UBO buffers
		uboBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<Buffer>(
				device,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			uboBuffers[i]->map();
		}

		// Global light layout
		lightSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		lightDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < lightDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*lightSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(lightDescriptorSets[i]);
		}

		// Global scene layout
		globalSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &shadowDescriptorImageInfo)
				.build(globalDescriptorSets[i]);
		}
	}
	void GlobalDescriptorsManager::updateUBO(uint32_t frameIndex, GlobalUBO ubo)
	{
		uboBuffers[frameIndex]->writeToBuffer(&ubo);
		uboBuffers[frameIndex]->flush();
	}
}