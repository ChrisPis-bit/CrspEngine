#pragma once
#include "Device.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace crsp {
	struct UIPushConstantData {
		glm::mat4 modelMatrix{ 1.0f };
	};


	struct UIRenderObject {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		glm::mat4 transformMatrix{ 1.0f };

	};

	class UIRenderer {
	public:
		UIRenderer(Device& device);
		~UIRenderer();

		UIRenderer(const UIRenderer&) = delete;
		UIRenderer operator=(const UIRenderer&) = delete;

		void render(FrameInfo& frameInfo, std::vector<UIRenderObject>& renderObjects);
	private:

		Device& device;
	};
}