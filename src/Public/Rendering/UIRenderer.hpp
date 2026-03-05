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
		Mesh* mesh;
		Material* material;
		
		UIPushConstantData transform;
	};

	/// <summary>
	/// Handles drawing 2D meshes to the screen.
	/// </summary>
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