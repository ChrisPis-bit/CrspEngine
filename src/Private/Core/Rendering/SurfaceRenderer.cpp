#include "Rendering/SurfaceRenderer.hpp"
#include "Rendering/SwapChain.hpp"

#include <stdexcept>
#include <algorithm>

namespace crsp {
	SurfaceRenderer::SurfaceRenderer(Device& device) : device(device)
	{
		instanceBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		stagingBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < instanceBuffers.size(); i++)
		{
			instanceBuffers[i] = std::make_unique<Buffer>(device, sizeof(InstanceData), MAX_INSTANCE_BUFFER_COUNT, 
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			stagingBuffers[i] = std::make_unique<Buffer>(device, sizeof(InstanceData), MAX_INSTANCE_BUFFER_COUNT,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}
	SurfaceRenderer::~SurfaceRenderer()
	{
	}
	void SurfaceRenderer::render(FrameInfo& frameInfo, std::vector<RenderObject>& renderObjects)
	{
		Material* prevMaterial = nullptr;
		Mesh* prevMesh = nullptr;

		std::sort(renderObjects.begin(), renderObjects.end());

		std::vector<InstanceData> instanceBatch;
		int currentOffset = 0;
		mapInstanceBuffer(frameInfo.frameIndex);

		for (auto& renderObject : renderObjects) {
			if (!renderObject.mesh || !renderObject.material)
				continue;

			// New batch start
			if (prevMaterial != renderObject.material || prevMesh != renderObject.mesh) {
				if (instanceBatch.size() != 0) {
					uploadToInstanceBuffer(instanceBatch, currentOffset, frameInfo.frameIndex);

					prevMesh->bindInstanced(frameInfo.commandBuffer, instanceBuffers[frameInfo.frameIndex]->getBuffer(), currentOffset * sizeof(InstanceData));
					prevMesh->draw(frameInfo.commandBuffer, instanceBatch.size(), 0);
					currentOffset += instanceBatch.size();
					instanceBatch.clear();
				}

				// Bind new material
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

				prevMesh = renderObject.mesh;
			}

			InstanceData instace{};
			instace.modelMatrix = renderObject.transformMatrix;
			instace.normalMatrix = renderObject.normalMatrix;

			//vkCmdPushConstants(frameInfo.commandBuffer,
			//	renderObject.material->getPipelineLayout(),
			//	VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			//	0,
			//	sizeof(ObjectPushConstantData),
			//	&push);
			instanceBatch.push_back(instace);
		}

		if (instanceBatch.size() != 0) {
			uploadToInstanceBuffer(instanceBatch, currentOffset, frameInfo.frameIndex);

			prevMesh->bindInstanced(frameInfo.commandBuffer, instanceBuffers[frameInfo.frameIndex]->getBuffer(), currentOffset * sizeof(InstanceData));
			prevMesh->draw(frameInfo.commandBuffer, instanceBatch.size(), 0);
			currentOffset += instanceBatch.size();
			instanceBatch.clear();
		}

		writeToInstanceBuffer(currentOffset, frameInfo.frameIndex);
	}
	void SurfaceRenderer::uploadToInstanceBuffer(std::vector<InstanceData>& instanceData, int currentOffset, int currentFrame)
	{
		uint32_t bufferSize = instanceData.size() * sizeof(InstanceData);
		uint32_t curOffset = currentOffset * sizeof(InstanceData);

		stagingBuffers[currentFrame]->writeToBuffer((void*)instanceData.data(), bufferSize, curOffset);

	}

	void SurfaceRenderer::mapInstanceBuffer(int currentFrame)
	{
		stagingBuffers[currentFrame]->map();
	}

	void SurfaceRenderer::writeToInstanceBuffer(int renderInstances, int currentFrame)
	{
		uint32_t bufferSize = renderInstances * sizeof(InstanceData);
		device.copyBuffer(stagingBuffers[currentFrame]->getBuffer(), instanceBuffers[currentFrame]->getBuffer(), bufferSize);
	}
} // namespace
