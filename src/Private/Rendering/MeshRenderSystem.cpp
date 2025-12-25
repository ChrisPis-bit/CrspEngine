#include "Rendering/MeshRenderSystem.hpp"

#include <stdexcept>

namespace crsp {
	MeshRenderSystem::MeshRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	MeshRenderSystem::~MeshRenderSystem()
	{
		vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
	}
	void MeshRenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		pipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		for (auto& gameObject : gameObjects) {
			if (!gameObject.mesh)
				continue;

			ObjectPushConstantData push{};
			gameObject.transform.rotation += glm::vec3(0, 0, 1.45f * frameInfo.frameTime);
			push.modelMatrix = gameObject.transform.calculateTransformationMatrix();
			push.normalMatrix = gameObject.transform.calculateNormalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(ObjectPushConstantData),
				&push);

			gameObject.mesh->bind(frameInfo.commandBuffer);
			gameObject.mesh->draw(frameInfo.commandBuffer);
		}
	}
	void MeshRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(ObjectPushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

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
	void MeshRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "cannot create pipeline before creating the pipeline layout!");

		pipeline = std::make_unique<Pipeline>(device, renderPass, pipelineLayout);
	}
} // namespace
