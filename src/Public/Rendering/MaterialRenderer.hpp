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
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		glm::mat4 transformMatrix {1.0f};
		glm::mat3 normalMatrix {1.0f};
	};

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