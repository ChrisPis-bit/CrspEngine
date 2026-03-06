#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

#include "Core/Rendering/Window.hpp"
#include "Core/Rendering/Device.hpp"
#include "Core/Rendering/Renderer.hpp"
#include "Core/Rendering/ShadowRenderer.hpp"
#include "Core/Rendering/UIRenderer.hpp"
#include "Core/Rendering/Descriptors.hpp"
#include "Core/SceneLogic/Scene.hpp"

namespace crsp {
	class App {
	public:
		App(std::unique_ptr<Scene> startScene);
		~App();

		void run();

		const int width = 1600;
		const int height = 900;

	private:
		void mainLoop();

		Window window{ width, height, "crsp" };
		Device device{ window };
		Renderer renderer{ window, device };
		std::unique_ptr<Scene> currentScene;
	};
}