#include "Rendering/UIRenderer.hpp"
#include "Rendering/MaterialRenderer.hpp"

namespace crsp {
	UIRenderer::UIRenderer(Device& device) : device(device)
	{
	}
	UIRenderer::~UIRenderer()
	{
	}

	void UIRenderer::render(FrameInfo& frameInfo, std::vector<UIRenderObject>& renderObjects) {
		for (auto& renderObject : renderObjects) {
			if (!renderObject.mesh || !renderObject.material)
				continue;

			renderObject.material->bindPipeline(frameInfo.commandBuffer);

			std::vector<VkDescriptorSet> descriptorSets{ frameInfo.globalDescriptorSet, renderObject.material->getDescriptorSet() };
			vkCmdBindDescriptorSets(frameInfo.commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				renderObject.material->getPipelineLayout(),
				0, static_cast<uint32_t>(descriptorSets.size()),
				descriptorSets.data(),
				0,
				nullptr);

			UIPushConstantData push{};
			push.modelMatrix = renderObject.transformMatrix;

			vkCmdPushConstants(frameInfo.commandBuffer,
				renderObject.material->getPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(UIPushConstantData),
				&push);

			renderObject.mesh->bind(frameInfo.commandBuffer);
			renderObject.mesh->draw(frameInfo.commandBuffer);
		}
	}
}