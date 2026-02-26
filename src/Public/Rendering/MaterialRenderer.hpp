#pragma once

#include "Device.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

#include <memory>

namespace crsp {

	struct ObjectPushConstantData {
		glm::mat4 normalMatrix{ 1.f };
		glm::mat4 modelMatrix{ 1.f };
	};

	struct RenderObject {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		glm::mat4 transformMatrix;
		glm::mat3 normalMatrix;
	};

	class MaterialRenderer {
	public:
		MaterialRenderer(Device& device);
		~MaterialRenderer();

		MaterialRenderer(const MaterialRenderer&) = delete;
		MaterialRenderer operator=(const MaterialRenderer&) = delete;

		void renderGameObjects(FrameInfo& frameInfo, std::vector<RenderObject>& renderObjects);

	private:

		Device& device;
	};
} // namespace