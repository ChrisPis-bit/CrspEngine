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
	Mesh::Mesh(Device& device, const Mesh::Builder& builder) : device(device)
	{
		createVertexBuffer(builder.vertices);
		createIndexBuffer(builder.indices);
	}

	Mesh::~Mesh()
	{
		
	}

	void Mesh::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer vertexBuffers[] = { vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		if (hasIndexBuffer()) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}
	}

	void Mesh::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer()) {
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexCount), 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertexCount), 1, 0, 0);
		}
	}

	std::unique_ptr<Mesh> Mesh::createMeshFromFile(Device& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<Mesh>(device, builder);
	}

	void Mesh::createVertexBuffer(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
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
		stagingBuffer.writeToBuffer((void*)vertices.data());

		// Creaty and copy data over to vertex buffer
		vertexBuffer = std::make_unique<Buffer>(device, 
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}
	void Mesh::createIndexBuffer(const std::vector<uint16_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());

		// Only create when there are more than 3 indices
		if (indexCount < 3)
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

		// Creaty and copy data over to vertex buffer
		indexBuffer = std::make_unique<Buffer>(device,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

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

		vertices.clear();
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
	}
} // namespace