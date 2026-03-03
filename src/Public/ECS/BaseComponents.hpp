#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/FontAtlas.hpp"

namespace crsp {
	struct Transform {
		glm::vec3 position{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		glm::mat4 calculateTransformationMatrix()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{position.x, position.y, position.z, 1.0f} };
		}

		glm::mat4 calculateNormalMatrix()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			const glm::vec3 invScale = 1.0f / scale;

			return glm::mat3{
				{
					invScale.x * (c1 * c3 + s1 * s2 * s3),
					invScale.x * (c2 * s3),
					invScale.x * (c1 * s2 * s3 - c3 * s1),
				},
				{
					invScale.y * (c3 * s1 * s2 - c1 * s3),
					invScale.y * (c2 * c3),
					invScale.y * (c1 * c3 * s2 + s1 * s3),
				},
				{
					invScale.z * (c2 * s1),
					invScale.z * (-s2),
					invScale.z * (c1 * c2),
				} };
		}
	};

	struct Transform2D {
		glm::vec2 position{};
		float width = 1.0f;
		float height = 1.0f;
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation{ 0.0f };

		float getAspect() { return width / height; }
	};

	struct MeshRender {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};

	struct TextRender {
		enum class Alignment {
			LEFT,
			CENTER
		};

		TextRender() {}
		TextRender(uint32_t maxCharacters, std::shared_ptr<Mesh> mesh) : maxCharacters(maxCharacters), mesh(mesh) {
			// initialize mesh
			std::vector<Vertex2D> vertices;
			std::vector<uint16_t> indices;
			vertices.resize(maxCharacters * 4);
			indices.resize(maxCharacters * 6);

			// Initiate index buffer
			for (size_t i = 0; i < maxCharacters; i++)
			{
				uint16_t indexBase = i * 6;
				uint16_t vertBase = i * 4;
				indices[indexBase] = vertBase;
				indices[indexBase + 1] = vertBase + 1;
				indices[indexBase + 2] = vertBase + 2;

				indices[indexBase + 3] = vertBase + 2;
				indices[indexBase + 4] = vertBase + 3;
				indices[indexBase + 5] = vertBase;
			}

			// Write index and empty vertex data
			float vertexSize = sizeof(Vertex2D);
			std::vector<uint8_t> rawDataVector;
			rawDataVector.resize(vertices.size() * vertexSize);
			std::memcpy(rawDataVector.data(), vertices.data(), rawDataVector.size());

			mesh->writeVertices(rawDataVector, vertexSize);
			mesh->writeIndices(indices);
		}

		bool textUpdated;

		float fontScale = 1;
		Alignment alignment = Alignment::LEFT;

		std::string text;
		uint32_t maxCharacters = 1024;

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		std::shared_ptr<FontAtlas> font;

		void set(const std::string& string) {
			text = string;
			textUpdated = true;
		}

		void setAlignment(Alignment alignment) {
			this->alignment = alignment;
			textUpdated = true;
		}

		void setFontScale(float scale) {
			fontScale = scale;
			textUpdated = true;
		}
	};
}