#include "App.hpp"

#include "Rendering/Mesh.hpp"
#include "Rendering/MeshRenderSystem.hpp"
#include "GameObject.hpp"

//std
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <memory>

namespace crsp {
	App::App() : window(width, height, "crsp"), device(window), renderer(window, device)
	{
	}

	App::~App()
	{
	}

	void App::run()
	{
		mainLoop();
	}

	void App::mainLoop()
	{
		// Prepare mesh and game object
		const std::vector<Vertex> vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		std::shared_ptr<Mesh> triangle = std::make_shared<Mesh>(device, vertices);

		GameObject triangleObject{};
		triangleObject.mesh = triangle;

		gameObjects.push_back(triangleObject);

		// prepare render system
		MeshRenderSystem renderSystem{device, renderer.getSwapChainRenderPass()};


		while (!window.shouldClose()) {
			glfwPollEvents();

			if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
				renderer.beginSwapChainRenderPass(commandBuffer);

				renderSystem.renderGameObjects(commandBuffer, gameObjects);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(device.getDevice());
	}
} // namespace