#include "App.hpp"

#include "Core/Rendering/Mesh.hpp"
#include "Core/Rendering/SurfaceRenderer.hpp"
#include "Core/Rendering/Texture.hpp"
#include "Core/Rendering/Material.hpp"
#include "Core/Rendering/GlobalDescriptorsManager.hpp"
#include "Core/Rendering/ResourceManager.hpp"
#include "Core/Rendering/DynamicBatcher.hpp"
#include "Core/SceneLogic/InputSystem.hpp"
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
		currentScene = std::make_unique<SnakeScene>();
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
		// shadow renderer
		ShadowRenderer shadowRenderer{ device };

		// Creates global descriptor pool and global descriptor sets/layouts
		GlobalDescriptorsManager globalDescriptorsManager(device, shadowRenderer.descriptorInfo());
		shadowRenderer.preparePipeline(globalDescriptorsManager.getLightDescriptorSetLayout());

		// Resource manager
		ResourceManager resourceManager{ device, renderer, globalDescriptorsManager };

		// Input
		InputSystem inputSystem{ window };

		// Initialize the scene
		currentScene->init(&resourceManager, &inputSystem, &window);

		// prepare render system
		SurfaceRenderer surfaceRenderer{ device };
		UIRenderer uiRenderer{ device };
		DynamicBatcher dynamicBatcher{ device };

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
			std::cout << deltaTime << '\n';

			// Update scene
			currentScene->tick(deltaTime, currentTime);

			// Camera movement
			float aspect = renderer.getAspectRatio();
			currentScene->updateCameraAspect(aspect);
			currentScene->getCamera().updateViewMatrix();

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
				RenderData& renderData = currentScene->getRenderData();

				// update UBO
				GlobalUBO ubo{};
				ubo.view = currentScene->getCamera().getView();
				ubo.proj = currentScene->getCamera().getProjection();
				ubo.lightSpaceMat = currentScene->getMainLight().getProjectionViewMatrix();
				ubo.lightDir = currentScene->getMainLight().getDirection();
				globalDescriptorsManager.updateUBO(frameIndex, ubo);

				// batch draw calls
				DynamicBatcher::FrameBatch frameBatch = dynamicBatcher.batchDraws(renderData.renderObjects);

				// shadow rendering
				shadowRenderer.beginShadowRenderPass(commandBuffer);
				shadowRenderer.draw(frameInfo, frameBatch);
				shadowRenderer.endShadowRenderPass(commandBuffer);

				// main rendering
				renderer.beginSwapChainRenderPass(commandBuffer);
				surfaceRenderer.render(frameInfo, frameBatch);
				uiRenderer.render(frameInfo, renderData.UIrenderObjects);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(device.getDevice());
	}
} // namespace