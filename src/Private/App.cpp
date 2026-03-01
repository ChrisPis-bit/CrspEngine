#include "App.hpp"

#include "Rendering/Mesh.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include "Camera.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/DirectionalLight.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/GlobalDescriptorsManager.hpp"
#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "SnakeGame/SnakeScene.hpp"

//std
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <memory>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace crsp {
	App::App()
	{
		currentScene = new SnakeScene();
	}

	App::~App()
	{
		delete(currentScene);
	}

	void App::run()
	{
		mainLoop();
	}

	void App::mainLoop()
	{
		// shadow renderer
		ShadowRenderer shadowRenderer{ device };

		// Creates global descriptor pool and global descriptor sets/layouts
		GlobalDescriptorsManager globalDescriptorsManager(device, 10, 10, 5, shadowRenderer.descriptorInfo());
		shadowRenderer.preparePipeline(globalDescriptorsManager.getLightDescriptorSetLayout());

		// Resource manager
		ResourceManager resourceManager{ device, renderer, globalDescriptorsManager };

		// Input
		InputSystem inputSystem{ window };

		// Initialize the scene
		currentScene->init(&resourceManager, &inputSystem);

		// prepare render system
		MaterialRenderer materialRenderer{ device };
		UIRenderer uiRenderer{ device };

		// Directional light
		DirectionalLight mainLight{};
		glm::vec3 lightDir = glm::normalize(glm::vec3(1.5, -3, -1));
		mainLight.setOrthographicProjection(-3.0f, 3.0f, -3.0f, 3.0f, 1.0f, 7.5f);

		// prepare start time
		auto lastTime = std::chrono::high_resolution_clock::now();
		auto startTime = lastTime;
		float lightAngle = 0;

		currentScene->start();

		while (!window.shouldClose()) {
			glfwPollEvents();

			// Update inputs after polling events
			inputSystem.update();

			// Deltatime
			auto newTime = std::chrono::high_resolution_clock::now();
			float currentTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime).count();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastTime).count();
			lastTime = newTime;

			// Update scene
			currentScene->tick(deltaTime, currentTime);

			// Camera movement
			float aspect = renderer.getAspectRatio();
			currentScene->updateCameraAspect(aspect);
			currentScene->getCamera().updateViewMatrix();

			// Rotate light
			if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_F))
				lightAngle += 0.5f * deltaTime;
			glm::vec3 lightDir = glm::normalize(glm::vec3(glm::cos(lightAngle), -3, glm::sin(lightAngle)));
			mainLight.setViewDirection(lightDir * 5.0f, lightDir, glm::vec3(0, 1, 0));

			if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
				// prepare frame info
				int frameIndex = renderer.getFrameIndex();

				FrameInfo frameInfo{
					frameIndex,
					deltaTime,
					commandBuffer,
					currentScene->getCamera(),
					globalDescriptorsManager.getGlobalDescriptorSet(frameIndex),
					globalDescriptorsManager.getLightDescriptorSet(frameIndex),
				};

				// Gather render data from game objects.
				RenderData renderData = currentScene->getRenderData();

				// update UBO
				GlobalUBO ubo{};
				ubo.view = currentScene->getCamera().getView();
				ubo.proj = currentScene->getCamera().getProjection();
				ubo.lightSpaceMat = mainLight.getProjectionViewMatrix();
				ubo.lightDir = lightDir;
				globalDescriptorsManager.updateUBO(frameIndex, ubo);

				// shadow rendering
				shadowRenderer.beginShadowRenderPass(commandBuffer);
				shadowRenderer.draw(frameInfo, renderData.renderObjects);
				shadowRenderer.endShadowRenderPass(commandBuffer);

				// main rendering
				renderer.beginSwapChainRenderPass(commandBuffer);
				materialRenderer.render(frameInfo, renderData.renderObjects);
				uiRenderer.render(frameInfo, renderData.UIrenderObjects);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(device.getDevice());
	}
} // namespace