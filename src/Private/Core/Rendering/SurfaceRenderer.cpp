#include "Rendering/SurfaceRenderer.hpp"
#include "Utils.hpp"

#include <stdexcept>
#include <algorithm>

namespace crsp {
	SurfaceRenderer::SurfaceRenderer(Device& device) : device(device)
	{
	}
	SurfaceRenderer::~SurfaceRenderer()
	{
	}
	void SurfaceRenderer::render(FrameInfo& frameInfo, DynamicBatcher::FrameBatch& frameBatch)
	{
		Material* prevMaterial = nullptr;

		for (auto& batch : frameBatch.batches) {
			// Bind only new material
			if (prevMaterial != batch.material) {
				batch.material->bindPipeline(frameInfo.commandBuffer);

				std::vector<VkDescriptorSet> descriptorSets{ frameInfo.globalDescriptorSet, batch.material->getDescriptorSet() };
				vkCmdBindDescriptorSets(frameInfo.commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					batch.material->getPipelineLayout(),
					0, static_cast<uint32_t>(descriptorSets.size()),
					descriptorSets.data(),
					0,
					nullptr);

				prevMaterial = batch.material;
			}

			// Draw batched
			batch.mesh->bindInstanced(frameInfo.commandBuffer, frameBatch.instanceBuffer, batch.offset * sizeof(InstanceData));
			batch.mesh->draw(frameInfo.commandBuffer, batch.instances, 0);
		}
	} // namespace
}
