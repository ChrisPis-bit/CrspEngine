#include "Rendering/ShadowRenderer.hpp"

#include <array>
#include <stdexcept>

namespace crsp {
	ShadowRenderer::ShadowRenderer(Device& device, VkDescriptorSetLayout lightSetLayout) : device(device), viewport{}, scissor{}
	{
		createDepthResources();
		createRenderPass();
		createPipelineLayout(lightSetLayout);
		createPipeline();
		createFramebuffers();
	}

	ShadowRenderer::~ShadowRenderer()
	{
		cleanupDepthResources();
		vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
		vkDestroyFramebuffer(device.getDevice(), frameBuffer, nullptr);
		vkDestroyRenderPass(device.getDevice(), renderPass, nullptr);
	}

	VkDescriptorImageInfo ShadowRenderer::descriptorInfo() {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		imageInfo.imageView = depthImageView;
		imageInfo.sampler = depthSampler;

		return imageInfo;
	}

	VkFormat ShadowRenderer::findDepthFormat()
	{
		return device.findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	void ShadowRenderer::createFramebuffers() {

		std::array<VkImageView, 1> attachments = {
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device.getDevice(), &framebufferInfo, nullptr, &frameBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void ShadowRenderer::beginShadowRenderPass(VkCommandBuffer commandBuffer)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = frameBuffer;

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { width, height };

		std::array<VkClearValue, 1> clearValues{};
		clearValues[0].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void ShadowRenderer::endShadowRenderPass(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffer);
	}

	void ShadowRenderer::draw(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		pipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1,
			&frameInfo.lightDescriptorSet,
			0,
			nullptr);

		for (auto& gameObject : gameObjects)
		{
			if (!gameObject.mesh)
				continue;

			ShadowPushConstantData push{};
			push.modelMatrix = gameObject.transform.calculateTransformationMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(ShadowPushConstantData),
				&push);

			gameObject.mesh->bind(frameInfo.commandBuffer);
			gameObject.mesh->draw(frameInfo.commandBuffer);
		}
	}

	void ShadowRenderer::createPipelineLayout(VkDescriptorSetLayout lightSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ShadowPushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ lightSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void ShadowRenderer::createPipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		//pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;

		pipelineConfig.dynamicStateEnables = {};
		pipelineConfig.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineConfig.dynamicStateInfo.pDynamicStates = nullptr;
		pipelineConfig.dynamicStateInfo.dynamicStateCount = 0;
		pipelineConfig.dynamicStateInfo.flags = 0;

		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = width;
		viewport.height = height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = { 0, 0 };
		scissor.extent = { width, height };

		pipelineConfig.viewportInfo.pViewports = &viewport;
		pipelineConfig.viewportInfo.pScissors = &scissor;

		pipeline = std::make_unique<Pipeline>(device,
			"shaders/shadow_shader.vert.spv",
			"shaders/shadow_shader.frag.spv",
			pipelineConfig);
	}

	void ShadowRenderer::createDepthResources()
	{
		depthFormat = findDepthFormat();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = depthFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0;

		device.createImageWithInfo(
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			depthImage,
			depthImageMemory);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = depthImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.getDevice(), &viewInfo, nullptr, &depthImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image view!");
		}

		// Get device properties for max anisotropy
		// TODO: possibly only do this once in device, and store results
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(device.getPhysicalDevice(), &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

		if (vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &depthSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void ShadowRenderer::createRenderPass()
	{
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 0;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 0;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		std::vector<VkSubpassDependency> dependencies = {
			 {
			   VK_SUBPASS_EXTERNAL,                            // uint32_t                   srcSubpass
			   0,                                              // uint32_t                   dstSubpass
			   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,          // VkPipelineStageFlags       srcStageMask
			   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,     // VkPipelineStageFlags       dstStageMask
			   VK_ACCESS_SHADER_READ_BIT,                      // VkAccessFlags              srcAccessMask
			   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,   // VkAccessFlags              dstAccessMask
			   VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
			 },
			 {
			   0,                                              // uint32_t                   srcSubpass
			   VK_SUBPASS_EXTERNAL,                            // uint32_t                   dstSubpass
			   VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,      // VkPipelineStageFlags       srcStageMask
			   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,          // VkPipelineStageFlags       dstStageMask
			   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,   // VkAccessFlags              srcAccessMask
			   VK_ACCESS_SHADER_READ_BIT,                      // VkAccessFlags              dstAccessMask
			   VK_DEPENDENCY_BY_REGION_BIT                     // VkDependencyFlags          dependencyFlags
			 }
		};

		std::array<VkAttachmentDescription, 1> attachments = { depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(device.getDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void ShadowRenderer::cleanupDepthResources()
	{
		vkDestroyImageView(device.getDevice(), depthImageView, nullptr);
		vkDestroyImage(device.getDevice(), depthImage, nullptr);
		vkFreeMemory(device.getDevice(), depthImageMemory, nullptr);
	}
}