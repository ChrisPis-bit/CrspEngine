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
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT * 2)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
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

		// Global light layout
		auto lightSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> lightDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < lightDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*lightSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(lightDescriptorSets[i]);
		}

		// shadow renderer
		ShadowRenderer shadowRenderer{ device, lightSetLayout->getDescriptorSetLayout() };

		// Global scene layout
		auto globalSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		auto textureInfo = texture.descriptorInfo();
		auto shadowInfo = shadowRenderer.descriptorInfo();

		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &textureInfo)
				.writeImage(2, &shadowInfo)
				.build(globalDescriptorSets[i]);
		}

		// Prepare mesh and game object
		std::shared_ptr<Mesh> vikingRoom = Mesh::createMeshFromFile(device, PROJECT_PATH + std::string("models/viking_room.obj"));
		std::shared_ptr<Mesh> quad = Mesh::createMeshFromFile(device, PROJECT_PATH + std::string("models/quad.obj"));

		GameObject vikingRoomObject{};
		vikingRoomObject.mesh = vikingRoom;
		vikingRoomObject.transform.position += glm::vec3(0.f, 0.0f, -3.0f);
		vikingRoomObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		gameObjects.push_back(std::move(vikingRoomObject));

		GameObject quadObject{};
		quadObject.mesh = quad;
		quadObject.transform.position += glm::vec3(1.5f, 0.0f, -5.0f);
		//quadObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		gameObjects.push_back(std::move(quadObject));


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
					globalDescriptorSets[frameIndex],
					lightDescriptorSets[frameIndex],
				};

				// update UBO
				GlobalUBO ubo{};
				ubo.view = camera.getView();
				ubo.proj = camera.getProjection();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// TODO: shadow rendering
				shadowRenderer.beginShadowRenderPass(commandBuffer);
				shadowRenderer.draw(frameInfo, gameObjects);
				shadowRenderer.endShadowRenderPass(commandBuffer);


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