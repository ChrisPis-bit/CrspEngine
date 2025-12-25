#include "Rendering/Mesh.hpp"

#include <stdexcept>

namespace crsp {
	Mesh::Mesh(Device& device, std::vector<Vertex> vertices) : vertices(vertices), device(device)
	{
		createVertexBuffer();
	}

	Mesh::Mesh(Device& device, std::vector<Vertex> vertices, std::vector<uint16_t> indices) : indices(indices), vertices(vertices), device(device)
	{
		createVertexBuffer();
		createIndexBuffer();
	}

	Mesh::~Mesh()
	{
		
	}

	void Mesh::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer vertexBuffers[] = { vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}
	}

	void Mesh::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		}
	}

	void Mesh::createVertexBuffer()
	{
		uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be atleast 3");

		uint32_t vertexSize = sizeof(vertices[0]);
		uint32_t bufferSize = vertexCount * vertexSize;

		// Creates staging buffer
		Buffer stagingBuffer{ device, 
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(vertices.data());

		// Creaty and copy data over to vertex buffer
		vertexBuffer = std::make_unique<Buffer>(device, 
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}
	void Mesh::createIndexBuffer()
	{
		// Only create when there are more than 3 indices
		if (indices.size() < 3)
			return;

		uint32_t indexCount = static_cast<uint32_t>(indices.size());
		assert(indexCount >= 3 && "Vertex count must be atleast 3");

		uint32_t indexSize = sizeof(indices[0]);
		uint32_t bufferSize = indexCount * indexSize;

		// Creates staging buffer
		Buffer stagingBuffer{ device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(indices.data());

		// Creaty and copy data over to vertex buffer
		indexBuffer = std::make_unique<Buffer>(device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);

		hasIndexBuffer = true;
	}
} // namespace