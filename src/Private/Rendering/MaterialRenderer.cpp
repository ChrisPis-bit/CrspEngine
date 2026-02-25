#include "Rendering/MaterialRenderer.hpp"

#include <stdexcept>

namespace crsp {
	MaterialRenderer::MaterialRenderer(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : device(device)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}
	MaterialRenderer::~MaterialRenderer()
	{
		vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
	}
	void MaterialRenderer::renderGameObjects(FrameInfo& frameInfo, std::vector<RenderObject>& renderObjects)
	{

		for (auto& renderObject : renderObjects) {
			if (!renderObject.mesh)
				continue;

			renderObject.material->bindPipeline(frameInfo.commandBuffer);

			std::vector<VkDescriptorSet> descriptorSets{ frameInfo.globalDescriptorSet, renderObject.material->getDescriptorSet()};
			vkCmdBindDescriptorSets(frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				renderObject.material->getPipelineLayout(),
				0, static_cast<uint32_t>(descriptorSets.size()),
				descriptorSets.data(),
				0,
				nullptr);

			ObjectPushConstantData push{};
			push.modelMatrix = renderObject.transformMatrix;
			push.normalMatrix = renderObject.normalMatrix;

			vkCmdPushConstants(frameInfo.commandBuffer,
				renderObject.material->getPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(ObjectPushConstantData),
				&push);

			renderObject.mesh->bind(frameInfo.commandBuffer);
			renderObject.mesh->draw(frameInfo.commandBuffer);
		}
	}
	void MaterialRenderer::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
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
	void MaterialRenderer::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "cannot create pipeline before creating the pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipelineConfig.multisampleInfo.rasterizationSamples = device.getMSAAsamples();
		pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;

		pipeline = std::make_unique<Pipeline>(device,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}
} // namespace
