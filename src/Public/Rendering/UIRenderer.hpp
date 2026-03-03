#pragma once
#include "Device.hpp"
#include "Pipeline.hpp"
#include "FrameInfo.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace crsp {
	struct UIPushConstantData {
		glm::vec2 position;
		glm::vec2 scale;
		float rotation;
	};


	struct UIRenderObject {
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		
		UIPushConstantData transform;
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