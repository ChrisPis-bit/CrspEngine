#include "Rendering/DynamicBatcher.hpp"
#include "Rendering/SurfaceRenderer.hpp"
#include "Utils.hpp"

#include <algorithm>


namespace crsp {
	DynamicBatcher::DynamicBatcher(Device& device) : device(device)
	{
		createInstanceBuffers(START_BUFFER_SIZE);
	}

	DynamicBatcher::FrameBatch DynamicBatcher::batchDraws(std::vector<RenderObject>& renderObjects)
	{
		// Map the staging buffer
		mapInstanceBuffer(renderObjects.size());

		// Prepare frame info
		FrameBatch frameBatch{};
		frameBatch.instanceBuffer = instanceBuffer->getBuffer();

		Material* prevMaterial = nullptr;
		Mesh* prevMesh = nullptr;

		// Sorts render objects based on their material and mesh
		std::sort(renderObjects.begin(), renderObjects.end());

		std::vector<InstanceData> instanceBatch;
		int currentOffset = 0;

		for (auto& renderObject : renderObjects) {
			if (!renderObject.mesh || !renderObject.material)
				continue;

			// New batch starts when previous material and mesh aren't the same anymore
			if (prevMaterial != renderObject.material || prevMesh != renderObject.mesh) {
				if (instanceBatch.size() != 0) {
					// Writes transform data to the staging buffer
					uploadToInstanceBuffer(instanceBatch, currentOffset);

					// Create new batch
					Batch batch{};
					batch.material = prevMaterial;
					batch.mesh = prevMesh;
					batch.instances = instanceBatch.size();
					batch.offset = currentOffset;

					frameBatch.batches.push_back(batch);

					// Increment current instance offset
					currentOffset += instanceBatch.size();
					instanceBatch.clear();
				}

				prevMaterial = renderObject.material;
				prevMesh = renderObject.mesh;
			}

			// Add new instance to the current recording batch
			InstanceData instace{};
			instace.modelMatrix = renderObject.transformMatrix;
			instace.normalMatrix = renderObject.normalMatrix;

			instanceBatch.push_back(instace);
		}

		// Add the last batch
		if (instanceBatch.size() != 0) {
			uploadToInstanceBuffer(instanceBatch, currentOffset);

			Batch batch{};
			batch.material = prevMaterial;
			batch.mesh = prevMesh;
			batch.instances = instanceBatch.size();
			batch.offset = currentOffset;

			frameBatch.batches.push_back(batch);

			currentOffset += instanceBatch.size();
			instanceBatch.clear();
		}

		// Copy staging buffer over to the device local buffer
		writeToInstanceBuffer(currentOffset);

		return frameBatch;
	}

	void DynamicBatcher::createInstanceBuffers(int instanceCount)
	{
		instanceBuffer = std::make_unique<Buffer>(device, sizeof(InstanceData), instanceCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		stagingBuffer = std::make_unique<Buffer>(device, sizeof(InstanceData), instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		currentBufferSize = instanceCount;
	}

	void DynamicBatcher::uploadToInstanceBuffer(std::vector<InstanceData>& instanceData, int currentOffset)
	{
		uint32_t bufferSize = instanceData.size() * sizeof(InstanceData);
		uint32_t curOffset = currentOffset * sizeof(InstanceData);

		stagingBuffer->writeToBuffer((void*)instanceData.data(), bufferSize, curOffset);
	}

	void DynamicBatcher::mapInstanceBuffer(int renderInstances)
	{
		// Grow buffers dynamically as instances increase
		if (renderInstances > currentBufferSize) {
			createInstanceBuffers(nextPowerOfTwo(renderInstances));
		}

		stagingBuffer->map();
	}

	void DynamicBatcher::writeToInstanceBuffer(int renderInstances)
	{
		uint32_t bufferSize = renderInstances * sizeof(InstanceData);
		device.copyBuffer(stagingBuffer->getBuffer(), instanceBuffer->getBuffer(), bufferSize);
	}
}