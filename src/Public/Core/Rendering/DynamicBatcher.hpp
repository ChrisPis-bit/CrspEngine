#pragma once
#include "Device.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

#include <vector>

namespace crsp {
	struct RenderObject;

	/// <summary>
	/// Handles dynamically uploading transform data to a buffer for instanced rendering.
	/// </summary>
	class DynamicBatcher {
	public:
		struct Batch {
			Mesh* mesh;
			Material* material;
			int offset;
			int instances;
		};

		struct FrameBatch {
			std::vector<Batch> batches;
			VkBuffer instanceBuffer;
		};

		constexpr static unsigned int START_BUFFER_SIZE = 128;

		DynamicBatcher(Device& device);

		/// <summary>
		/// Batches draws based on objects with the same mesh and material.
		/// </summary>
		/// <param name="renderObjects">Objects to render</param>
		/// <returns>Current frame batch data</returns>
		FrameBatch batchDraws(std::vector<RenderObject>& renderObjects);

	private:
		void createInstanceBuffers(int instanceCount);
		void uploadToInstanceBuffer(std::vector<InstanceData>& instanceData, int currentOffset);
		void mapInstanceBuffer(int renderInstances);
		void writeToInstanceBuffer(int renderInstances);

		std::unique_ptr<Buffer> instanceBuffer;
		std::unique_ptr<Buffer> stagingBuffer;
		int currentBufferSize = START_BUFFER_SIZE;

		Device& device;
	};
}