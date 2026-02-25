#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

#include "Rendering/Window.hpp"
#include "Rendering/Device.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/ShadowRenderer.hpp"
#include "Rendering/Descriptors.hpp"
#include "GameObject.hpp"

namespace crsp {
	struct BaseMaterial {
		glm::vec4 color;
	};

	class App {
	public:
		App();
		~App();

		void run();

		const int width = 800;
		const int height = 600;

	private:
		void mainLoop();

		Window window{ width, height, "crsp" };
		Device device{ window };
		Renderer renderer{ window, device };

		std::vector<GameObject> gameObjects;

	};
}