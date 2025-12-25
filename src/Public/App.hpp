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
#include "GameObject.hpp"

namespace crsp {


	class App {
	public:
		App();
		~App();

		void run();

		const uint32_t width = 800;
		const uint32_t height = 600;

	private:
		void mainLoop();

		Window window;
		Device device;
		Renderer renderer;

		std::vector<GameObject> gameObjects;
	};
}