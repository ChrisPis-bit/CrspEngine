#include "SnakeGame/SnakeScene.hpp"

#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace crsp {
	void SnakeScene::loadResources()
	{
		// Load resources
		std::shared_ptr<Texture2D> vikingRoomTexture = resourceManager->loadTexture("textures/viking_room.png", "viking_room");
		resourceManager->loadMesh("models/viking_room.obj", "viking_room");
		resourceManager->loadMesh("models/quad.obj", "quad");

		// Create materials
		std::shared_ptr<Material> standardMaterial = resourceManager->createMaterial(sizeof(BaseMaterial), 1,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			"standard");
		standardMaterial->writeImage(0, &vikingRoomTexture->descriptorInfo());
		BaseMaterial baseMat{};
		baseMat.color = glm::vec4(1.0, .5, .1, 1.0);
		standardMaterial->writeUniform(&baseMat);
		standardMaterial->build();

		std::shared_ptr<Material> standardMaterial_test = resourceManager->createMaterial(sizeof(BaseMaterial), 1,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			"standard_test");
		standardMaterial_test->writeImage(0, &vikingRoomTexture->descriptorInfo());
		BaseMaterial baseMat2{};
		baseMat2.color = glm::vec4(0.2, 1.0, .1, 1.0);
		standardMaterial_test->writeUniform(&baseMat2);
		standardMaterial_test->build();
	}

	void SnakeScene::spawnObjects()
	{
		// Create gameobjects
		GameObject vikingRoomObject{};
		vikingRoomObject.mesh = resourceManager->getMesh("viking_room");
		vikingRoomObject.transform.position += glm::vec3(0.0f, 0.0f, 0.0f);
		vikingRoomObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		vikingRoomObject.material = resourceManager->getMaterial("standard");

		GameObject quadObject{};
		quadObject.mesh = resourceManager->getMesh("quad");
		quadObject.transform.position = glm::vec3(1.5f, 0.0f, 0.0f);
		quadObject.material = resourceManager->getMaterial("standard");
		//quadObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);

		GameObject groundObject{};
		groundObject.mesh = resourceManager->getMesh("quad");
		groundObject.transform.position = vikingRoomObject.transform.position + glm::vec3(0.0f, 0.0f, 0.0f);
		groundObject.transform.scale = glm::vec3(5.0f, 5.0f, 5.0f);
		groundObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		groundObject.material = resourceManager->getMaterial("standard_test");

		gameObjects.push_back(std::move(vikingRoomObject));
		gameObjects.push_back(std::move(quadObject));
		gameObjects.push_back(std::move(groundObject));
	}

	void SnakeScene::start()
	{
		Scene::start();
	}

	void SnakeScene::update(float deltaTime, float totalTime)
	{
		Scene::update(deltaTime, totalTime);

		BaseMaterial baseMat2{};
		baseMat2.color = glm::vec4(glm::sin(totalTime), .5, .1, 1.0);
		resourceManager->getMaterial("standard_test")->writeUniform(&baseMat2);
	}
}