#pragma once

#include "Device.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

#include <memory>

namespace crsp {

	struct ObjectPushConstantData {
		glm::mat4 normalMatrix{ 1.0f };
		glm::mat4 modelMatrix{ 1.0f };
	};

	struct RenderObject {
		RenderObject(Mesh* mesh, Material* material) : mesh(mesh), material(material) {
			if (!mesh || !material)
				key = 0; // Invalid render obj

			key = (uint32_t)material->getId() << 16 | (uint32_t)mesh->getId();
		}

		bool operator<(const RenderObject& other) const {
			return key < other.key;
		}

		uint32_t key;
		Mesh* mesh;
		Material* material;
		glm::mat4 transformMatrix {1.0f};
		glm::mat3 normalMatrix {1.0f};
	};


	/// <summary>
	/// Handles drawing 3D meshes to the screen.
	/// </summary>
	class MaterialRenderer {
	public:
		MaterialRenderer(Device& device);
		~MaterialRenderer();

		MaterialRenderer(const MaterialRenderer&) = delete;
		MaterialRenderer operator=(const MaterialRenderer&) = delete;

		void render(FrameInfo& frameInfo, std::vector<RenderObject>& renderObjects);

	private:

		Device& device;
	};
} // namespace