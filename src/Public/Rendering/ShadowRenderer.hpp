#pragma once
#include "Device.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "GameObject.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include <memory>

namespace crsp {
	class ShadowRenderer {
		struct ShadowPushConstantData {
			glm::mat4 modelMatrix;
		};

	public:
		ShadowRenderer(Device& device, VkDescriptorSetLayout lightSetLayout);
		ShadowRenderer(Device& device);
		~ShadowRenderer();

		ShadowRenderer(const ShadowRenderer&) = delete;
		ShadowRenderer& operator=(const ShadowRenderer&) = delete;

		void preparePipeline(VkDescriptorSetLayout lightSetLayout);

		VkDescriptorImageInfo descriptorInfo();

		void beginShadowRenderPass(VkCommandBuffer commandBuffer);
		void endShadowRenderPass(VkCommandBuffer commandBuffer);

		void draw(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

		const uint32_t width = 1024, height = 1024;

	private:
		void createDepthResources();
		void createFramebuffers();
		void createRenderPass();
		void createPipeline();
		void createPipelineLayout(VkDescriptorSetLayout lightSetLayout);

		void cleanupDepthResources();


		VkFormat findDepthFormat();

		Device& device;
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout pipelineLayout;

		VkRenderPass renderPass;

		VkImage depthImage;
		VkFormat depthFormat;
		VkImageView depthImageView;
		VkDeviceMemory depthImageMemory;
		VkSampler depthSampler;

		VkFramebuffer frameBuffer;

		VkViewport viewport{};
		VkRect2D scissor{};
	};
}