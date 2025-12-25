#include "Rendering/MeshRenderSystem.hpp"

#include <stdexcept>

namespace crsp {
	MeshRenderSystem::MeshRenderSystem(Device& device, VkRenderPass renderPass) : device(device)
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}
	MeshRenderSystem::~MeshRenderSystem()
	{
		vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
	}
	void MeshRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects)
	{
		pipeline->bind(commandBuffer);
		for (auto& gameObject : gameObjects) {
			if (!gameObject.mesh)
				continue;

			gameObject.mesh->bind(commandBuffer);
			gameObject.mesh->draw(commandBuffer);
		}
	}
	void MeshRenderSystem::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void MeshRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "cannot create pipeline before creating the pipeline layout!");

		pipeline = std::make_unique<Pipeline>(device, renderPass, pipelineLayout);
	}
} // namespace
