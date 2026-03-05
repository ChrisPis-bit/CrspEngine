#include "Rendering/MaterialRenderer.hpp"

#include <stdexcept>
#include <algorithm>

namespace crsp {
	MaterialRenderer::MaterialRenderer(Device& device) : device(device)
	{
	}
	MaterialRenderer::~MaterialRenderer()
	{
	}
	void MaterialRenderer::render(FrameInfo& frameInfo, std::vector<RenderObject>& renderObjects)
	{
		Material* prevMaterial = nullptr;
		Mesh* prevMesh = nullptr;

		std::sort(renderObjects.begin(), renderObjects.end());

		for (auto& renderObject : renderObjects) {
			if (!renderObject.mesh || !renderObject.material)
				continue;

			if (prevMaterial != renderObject.material) {
				renderObject.material->bindPipeline(frameInfo.commandBuffer);

				std::vector<VkDescriptorSet> descriptorSets{ frameInfo.globalDescriptorSet, renderObject.material->getDescriptorSet() };
				vkCmdBindDescriptorSets(frameInfo.commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					renderObject.material->getPipelineLayout(),
					0, static_cast<uint32_t>(descriptorSets.size()),
					descriptorSets.data(),
					0,
					nullptr);

				prevMaterial = renderObject.material;
			}

			ObjectPushConstantData push{};
			push.modelMatrix = renderObject.transformMatrix;
			push.normalMatrix = renderObject.normalMatrix;

			vkCmdPushConstants(frameInfo.commandBuffer,
				renderObject.material->getPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(ObjectPushConstantData),
				&push);

			if (prevMesh != renderObject.mesh) {
				renderObject.mesh->bind(frameInfo.commandBuffer);
				prevMesh = renderObject.mesh;
			}
			renderObject.mesh->draw(frameInfo.commandBuffer);
		}
	}
} // namespace
