#include "App.hpp"

#include "Rendering/Mesh.hpp"
#include "Rendering/MeshRenderSystem.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "Rendering/Texture.hpp"

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
		globalPool = DescriptorPool::Builder(device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
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
		Texture2D texture{ device };

		std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<Buffer>(
				device,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		auto textureInfo = texture.descriptorInfo();

		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &textureInfo)
				.build(globalDescriptorSets[i]);
		}

		// Prepare mesh and game object
		std::shared_ptr<Mesh> triangle = Mesh::createMeshFromFile(device, PROJECT_PATH + std::string("models/viking_room.obj"));

		GameObject vikingRoomObject{};
		vikingRoomObject.mesh = triangle;
		vikingRoomObject.transform.position += glm::vec3(.5f, 0.0f, -5.0f);
		vikingRoomObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		//vikingRoomObject.transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
		gameObjects.push_back(std::move(vikingRoomObject));
		GameObject triangleObject2{};
		//triangleObject2.mesh = triangle;
		//triangleObject2.transform.position += glm::vec3(.5f, 0.0f, -14.0f);
		//triangleObject2.transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		//triangleObject2.transform.scale = glm::vec3(5.0f, 1.0f, 5.0f);
		//gameObjects.push_back(std::move(triangleObject2));

		// prepare render system
		MeshRenderSystem renderSystem{device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

		// Camera
		Camera camera{};

		auto lastTime = std::chrono::high_resolution_clock::now();
		while (!window.shouldClose()) {
			glfwPollEvents();

			// Deltatime
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastTime).count();
			lastTime = newTime;

			// Camera movement
			float aspect = renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, .1f, 100.0f);
			camera.move(window.getGLFWwindow(), frameTime);
			camera.updateViewMatrix();

			if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
				// prepare frame info
				int frameIndex = renderer.getFrameIndex();

				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};

				// update UBO
				GlobalUBO ubo{};
				ubo.view = camera.getView();
				ubo.proj = camera.getProjection();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// TODO: shadow rendering

				// render
				renderer.beginSwapChainRenderPass(commandBuffer);

				renderSystem.renderGameObjects(frameInfo, gameObjects);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(device.getDevice());
	}
} // namespace