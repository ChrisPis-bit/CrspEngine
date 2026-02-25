#include "App.hpp"

#include "Rendering/Mesh.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/DirectionalLight.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/GlobalDescriptorsManager.hpp"

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

		// shadow renderer
		ShadowRenderer shadowRenderer{ device };

		// Creates global descriptor pool and global descriptor sets/layouts
		GlobalDescriptorsManager globalDescriptorsManager(device, 2, 2, 1, shadowRenderer.descriptorInfo());
		shadowRenderer.preparePipeline(globalDescriptorsManager.getLightDescriptorSetLayout());

		// Create Materials
		auto textureInfo = texture.descriptorInfo();
		std::shared_ptr<Material> material = std::make_unique<Material>(device, renderer.getSwapChainRenderPass(), sizeof(BaseMaterial), 1, *globalDescriptorsManager.globalPool, globalDescriptorsManager.getGlobalDescriptorSetLayout(),
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv");
		material->writeImage(0, &textureInfo);
		BaseMaterial baseMat{};
		baseMat.color = glm::vec4(1.0, .5, .1, 1.0);
		material->writeUniform(&baseMat);
		material->build();

		std::shared_ptr<Material> material2 = std::make_unique<Material>(device, renderer.getSwapChainRenderPass(), sizeof(BaseMaterial), 1, *globalDescriptorsManager.globalPool, globalDescriptorsManager.getGlobalDescriptorSetLayout(),
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv");
		material2->writeImage(0, &textureInfo);
		BaseMaterial baseMat2{};
		baseMat2.color = glm::vec4(0.0, .5, .1, 1.0);
		material2->writeUniform(&baseMat2);
		material2->build();

		// Prepare meshes
		std::shared_ptr<Mesh> vikingRoom = Mesh::createMeshFromFile(device, PROJECT_PATH + std::string("models/viking_room.obj"));
		std::shared_ptr<Mesh> quad = Mesh::createMeshFromFile(device, PROJECT_PATH + std::string("models/quad.obj"));

		// Create gameobjects
		GameObject vikingRoomObject{};
		vikingRoomObject.mesh = vikingRoom;
		vikingRoomObject.transform.position += glm::vec3(0.0f, 0.0f, 0.0f);
		vikingRoomObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		vikingRoomObject.material = material;

		GameObject quadObject{};
		quadObject.mesh = quad;
		quadObject.transform.position = glm::vec3(1.5f, 0.0f, 0.0f);
		quadObject.material = material;
		//quadObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);

		GameObject groundObject{};
		groundObject.mesh = quad;
		groundObject.transform.position = vikingRoomObject.transform.position + glm::vec3(0.0f, 0.0f, 0.0f);
		groundObject.transform.scale = glm::vec3(5.0f, 5.0f, 5.0f);
		groundObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		groundObject.material = material2;

		gameObjects.push_back(std::move(vikingRoomObject));
		gameObjects.push_back(std::move(quadObject));
		gameObjects.push_back(std::move(groundObject));

		// prepare render system
		MaterialRenderer materialRenderer{device, renderer.getSwapChainRenderPass(), globalDescriptorsManager.getGlobalDescriptorSetLayout() };

		// Camera
		Camera camera{};
		camera.position = glm::vec3(0.0f, -1.0f, 1.0f);

		// Directional light
		DirectionalLight mainLight{};
		glm::vec3 lightDir = glm::normalize(glm::vec3(1.5, -3, -1));
		mainLight.setOrthographicProjection(-3.0f, 3.0f, -3.0f, 3.0f, 1.0f, 7.5f);

		auto lastTime = std::chrono::high_resolution_clock::now();
		auto startTime = lastTime;
		float lightAngle = 0;
		while (!window.shouldClose()) {
			glfwPollEvents();

			// Deltatime
			auto newTime = std::chrono::high_resolution_clock::now();
			float currentTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - startTime).count();
			float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - lastTime).count();
			lastTime = newTime;

			// Camera movement
			float aspect = renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, .1f, 100.0f);
			camera.move(window.getGLFWwindow(), deltaTime);
			camera.updateViewMatrix();

			// Rotate light
			if(glfwGetKey(window.getGLFWwindow(), GLFW_KEY_F))
			lightAngle += 0.5f * deltaTime;
			glm::vec3 lightDir = glm::normalize(glm::vec3(glm::cos(lightAngle), -3, glm::sin(lightAngle)));
			mainLight.setViewDirection(lightDir * 5.0f, lightDir, glm::vec3(0, 1, 0));

			if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
				BaseMaterial baseMat2{};
				baseMat2.color = glm::vec4(glm::sin(currentTime), .5, .1, 1.0);
				material2->writeUniform(&baseMat2);
				// prepare frame info
				int frameIndex = renderer.getFrameIndex();

				FrameInfo frameInfo{
					frameIndex,
					deltaTime,
					commandBuffer,
					camera,
					globalDescriptorsManager.getGlobalDescriptorSet(frameIndex),
					globalDescriptorsManager.getLightDescriptorSet(frameIndex),
				};

				// Gather render data from game objects.
				std::vector<RenderObject> renderObjects(gameObjects.size());
				for (size_t i = 0; i < gameObjects.size(); i++)
				{
					renderObjects[i] = gameObjects[i].renderData();
				}

				// update UBO
				GlobalUBO ubo{};
				ubo.view = camera.getView();
				ubo.proj = camera.getProjection();
				ubo.lightSpaceMat = mainLight.getProjectionViewMatrix();
				ubo.lightDir = lightDir;
				globalDescriptorsManager.updateUBO(frameIndex, ubo);

				// shadow rendering
				shadowRenderer.beginShadowRenderPass(commandBuffer);
				shadowRenderer.draw(frameInfo, gameObjects);
				shadowRenderer.endShadowRenderPass(commandBuffer);

				// main rendering
				renderer.beginSwapChainRenderPass(commandBuffer);

				materialRenderer.renderGameObjects(frameInfo, renderObjects);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(device.getDevice());
	}
} // namespace