#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "Rendering/Mesh.hpp"
#include "Utils.hpp"

#include <stdexcept>
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
	template<>
	struct hash<crsp::Vertex> {
		size_t operator()(crsp::Vertex const& vertex) const {
			size_t seed = 0;
			crsp::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.texCoord);
			return seed;
		}
	};
}

namespace crsp {
	uint16_t Mesh::nextId = 1;

	Mesh::Mesh(Device& device, const Mesh::Builder& builder, Type type) : device(device), type(type)
	{
		// Increment unique Id
		id = nextId;
		nextId++;

		if (builder.vertexSize == 0)
			return;

		createVertexBuffer(builder.vertexBuffer, builder.vertexSize);
		createIndexBuffer(builder.indices);
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer vertexBuffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		if (hasIndexBuffer()) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}
	}

	void Mesh::bindInstanced(VkCommandBuffer commandBuffer, VkBuffer instanceBuffer, VkDeviceSize instanceOffset)
	{
		VkBuffer vertexBuffers[] = { vertexBuffer->getBuffer(), instanceBuffer };
		VkDeviceSize offsets[] = { 0 , instanceOffset };
		vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);

		if (hasIndexBuffer()) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}
	}

	void Mesh::draw(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance)
	{
		if (hasIndexBuffer()) {
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexCount), instanceCount, 0, 0, firstInstance);
		}
		else {
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexCount), instanceCount, 0, firstInstance);
		}
	}

	void Mesh::writeVertices(const std::vector<uint8_t>& vertices, uint32_t vertexSize)
	{
		if (vertexCount != (vertices.size() / (vertexSize / sizeof(uint8_t))))
			createVertexBuffer(vertices, vertexSize);
		else
			writeVertexBuffer(vertices, vertexSize);
	}

	void Mesh::writeIndices(const std::vector<uint16_t>& indices)
	{
		if (indexCount != indices.size())
			createIndexBuffer(indices);
		else
			writeIndexBuffer(indices);
	}

	std::unique_ptr<Mesh> Mesh::createMeshFromFile(Device& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<Mesh>(device, builder);
	}

	void Mesh::createVertexBuffer(const std::vector<uint8_t>& vertices, uint32_t vertexSize)
	{
		vertexCount = static_cast<uint32_t>(vertices.size() / (vertexSize / sizeof(uint8_t)));
		assert(vertexCount >= 3 && "Vertex count must be atleast 3");

		// Create vertex buffer
		switch (type)
		{
		case crsp::Mesh::Type::DYNAMIC: // Write directly to host coherent buffer
			vertexBuffer = std::make_unique<Buffer>(device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			break;
		default: // Default, accounts for STATIC
			vertexBuffer = std::make_unique<Buffer>(device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			break;
		}

		writeVertexBuffer(vertices, vertexSize);
	}
	void Mesh::createIndexBuffer(const std::vector<uint16_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());

		// Only create when there are more than 3 indices
		if (indexCount < 3)
			return;

		assert(indexCount >= 3 && "Vertex count must be atleast 3");

		uint32_t indexSize = sizeof(indices[0]);

		// Creat index buffer
		indexBuffer = std::make_unique<Buffer>(device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		writeIndexBuffer(indices);
	}

	void Mesh::writeVertexBuffer(const std::vector<uint8_t>& vertices, uint32_t vertexSize)
	{
		if (vertexCount != (vertices.size() / (vertexSize / sizeof(uint8_t))))
			return;

		assert(vertexCount >= 3 && "Vertex count must be atleast 3");

		switch (type)
		{
		case crsp::Mesh::Type::DYNAMIC:
			vertexBuffer->map();
			vertexBuffer->writeToBuffer((void*)vertices.data());
			break;
		default: // Default, accounts for STATIC

			uint32_t bufferSize = vertexCount * vertexSize;

			// Creates staging buffer
			Buffer stagingBuffer{ device,
				vertexSize,
				vertexCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};

			stagingBuffer.map();
			stagingBuffer.writeToBuffer((void*)vertices.data());

			device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
			break;
		}
	}

	void Mesh::writeIndexBuffer(const std::vector<uint16_t>& indices)
	{
		// Only write when the index count is equal to the given indices
		if (indices.size() != indexCount)
			return;

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
		stagingBuffer.writeToBuffer((void*)indices.data());

		device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void Mesh::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<Vertex> vertices;
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};

				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.texCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

		copyToVertexBuffer(vertices.data(), vertices.size(), sizeof(Vertex));
	}

	void Mesh::Builder::copyToVertexBuffer(void* src, uint32_t vertexCount, uint32_t vertexSize)
	{
		this->vertexSize = vertexSize;
		vertexBuffer.resize(vertexCount * vertexSize);
		std::memcpy(vertexBuffer.data(), src, vertexBuffer.size());
	}
} // namespace