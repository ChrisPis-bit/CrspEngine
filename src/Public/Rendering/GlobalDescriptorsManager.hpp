#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Rendering/Device.hpp"
#include "Rendering/Descriptors.hpp"
#include <Rendering/Buffer.hpp>

#include <memory>

namespace crsp {
	struct GlobalUBO {
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 lightSpaceMat;
		glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -3.0f, -1.0f));
	};
	class GlobalDescriptorsManager {
	public:
		GlobalDescriptorsManager(Device& device, uint32_t maxSets, uint32_t bufferCount, uint32_t textureCount, VkDescriptorImageInfo shadowDescriptorImageInfo);

		VkDescriptorSet getGlobalDescriptorSet(uint32_t frameIndex) { return globalDescriptorSets[frameIndex]; }
		VkDescriptorSet getLightDescriptorSet(uint32_t frameIndex) { return lightDescriptorSets[frameIndex]; }

		VkDescriptorSetLayout getGlobalDescriptorSetLayout() { return globalSetLayout->getDescriptorSetLayout(); }
		VkDescriptorSetLayout getLightDescriptorSetLayout() { return lightSetLayout->getDescriptorSetLayout(); }

		void updateUBO(uint32_t frameIndex, GlobalUBO ubo);

		std::unique_ptr<DescriptorPool> globalPool{};

	private:
		Device& device;

		std::unique_ptr<DescriptorSetLayout> lightSetLayout;
		std::unique_ptr<DescriptorSetLayout> globalSetLayout;

		std::vector<VkDescriptorSet> lightDescriptorSets;
		std::vector<VkDescriptorSet> globalDescriptorSets;

		std::vector<std::unique_ptr<Buffer>> uboBuffers;
	};
}