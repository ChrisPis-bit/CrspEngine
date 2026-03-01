#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Device.hpp"
#include "Buffer.hpp"

#include <array>
#include <vector>
#include <memory>

namespace crsp {

	struct Vertex {
		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec3 color{};
		glm::vec2 texCoord{};

		static VkVertexInputBindingDescription getBindingDescriptions() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			attributeDescriptions.push_back({
				0, // location
				0, // binding
				VK_FORMAT_R32G32B32_SFLOAT, // format
				offsetof(Vertex, position) // offset
				});

			attributeDescriptions.push_back({
				1,
				0,
				VK_FORMAT_R32G32B32_SFLOAT,
				offsetof(Vertex, normal)
				});

			attributeDescriptions.push_back({
				2,
				0,
				VK_FORMAT_R32G32B32_SFLOAT,
				offsetof(Vertex, color)
				});

			attributeDescriptions.push_back({
				3,
				0,
				VK_FORMAT_R32G32_SFLOAT,
				offsetof(Vertex, texCoord)
				});

			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
		}
	};

	struct Vertex2D {
		glm::vec2 position{};
		glm::vec3 color{};
		glm::vec2 texCoord{};

		static VkVertexInputBindingDescription getBindingDescriptions() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex2D);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

			attributeDescriptions.push_back({
				0, // location
				0, // binding
				VK_FORMAT_R32G32_SFLOAT, // format
				offsetof(Vertex2D, position) // offset
				});

			attributeDescriptions.push_back({
				1,
				0,
				VK_FORMAT_R32G32B32_SFLOAT,
				offsetof(Vertex2D, color)
				});

			attributeDescriptions.push_back({
				2,
				0,
				VK_FORMAT_R32G32_SFLOAT,
				offsetof(Vertex2D, texCoord)
				});

			return attributeDescriptions;
		}

		bool operator==(const Vertex2D& other) const {
			return position == other.position && color == other.color && texCoord == other.texCoord;
		}
	};

	class Mesh {
	public:
		struct Builder {
			uint32_t vertexSize;
			std::vector<uint8_t> vertexBuffer{};
			std::vector<uint16_t> indices{};

			void loadModel(const std::string& filepath);
			void copyToVertexBuffer(void* src, uint32_t vertexCount, uint32_t vertexSize);
		};

		Mesh(Device& device, const Mesh::Builder& builder);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::unique_ptr<Mesh> createMeshFromFile(Device& device, const std::string& filepath);

	private:
		void createVertexBuffer(const std::vector<uint8_t>& vertices, uint32_t vertexSize);
		void createIndexBuffer(const std::vector<uint16_t>& indices);

		bool hasIndexBuffer() { return indexCount > 3; };

		Device& device;

		std::unique_ptr<Buffer> vertexBuffer;
		uint32_t vertexCount = 0;
		std::unique_ptr<Buffer> indexBuffer;
		uint32_t indexCount = 0;
	};
} // namespace