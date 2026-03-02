#include "SnakeGame/SnakeScene.hpp"
#include "SnakeGame/SnakeComponents.hpp"
#include "ECS/Systems/MeshRenderSystem.hpp"
#include "SnakeGame/Systems/GridTransformSystem.hpp"
#include "SnakeGame/Systems/SnakeControllerSystem.hpp"
#include "Utils.hpp"

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
		resourceManager->loadMesh("models/cube.obj", "cube");

		// Create materials
		std::shared_ptr<Material> standardMaterial = resourceManager->createMaterial(sizeof(BaseMaterial), 1, 
			Material::RenderDomain::Surface3D,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			"standard");
		standardMaterial->writeImage(0, &resourceManager->atlasTexture->descriptorInfo());
		BaseMaterial baseMat{};
		baseMat.color = glm::vec4(1.0, .5, .1, 1.0);
		standardMaterial->writeUniform(&baseMat);
		standardMaterial->build();

		std::shared_ptr<Material> snakeMaterial = resourceManager->createMaterial(sizeof(BaseMaterial), 0, 
			Material::RenderDomain::Surface3D,
			"shaders/lit_color.vert.spv",
			"shaders/lit_color.frag.spv",
			"snake");
		BaseMaterial snakeMat{};
		snakeMat.color = glm::vec4(0.2, 1.0, .1, 1.0);
		snakeMaterial->writeUniform(&snakeMat);
		snakeMaterial->build();

		std::shared_ptr<Material> appleMaterial = resourceManager->createMaterial(sizeof(BaseMaterial), 0, 
			Material::RenderDomain::Surface3D,
			"shaders/lit_color.vert.spv",
			"shaders/lit_color.frag.spv",
			"apple");
		BaseMaterial appleMat{};
		appleMat.color = glm::vec4(1.0, 0.0, 0.0, 1.0);
		appleMaterial->writeUniform(&appleMat);
		appleMaterial->build();

		std::shared_ptr<Material> textMaterial = resourceManager->createMaterial(0, 0, 
			Material::RenderDomain::UI,
			"shaders/text.vert.spv",
			"shaders/text.frag.spv",
			"text");
		textMaterial->build();
	}

	void SnakeScene::registerSystems()
	{
		// register systems
		meshRenderSystem = entityManager.registerSystem<MeshRenderSystem>();
		entityManager.registerSystem<SnakeControllerSystem>(grid, *inputSystem, *resourceManager);
		entityManager.registerSystem<GridTransformSystem>(grid);
	}

	void SnakeScene::spawnEntities()
	{
		// Create initial entities
		appleEntity = entityManager.createEntity();
		Transform* transform = entityManager.addComponent<Transform>(appleEntity);
		transform->scale = glm::vec3(.5f);
		MeshRender* meshRender = entityManager.addComponent<MeshRender>(appleEntity);
		meshRender->material = resourceManager->getMaterial("apple").get();
		meshRender->mesh = resourceManager->getMesh("cube").get();
		entityManager.addComponent<GridTransform>(appleEntity);


		snakeEntity = entityManager.createEntity();
		transform = entityManager.addComponent<Transform>(snakeEntity);
		transform->scale = glm::vec3(.5f);
		meshRender = entityManager.addComponent<MeshRender>(snakeEntity);
		meshRender->material = resourceManager->getMaterial("snake").get();
		meshRender->mesh = resourceManager->getMesh("cube").get();
		entityManager.addComponent<GridTransform>(snakeEntity);
		entityManager.addComponent<SnakeHead>(snakeEntity)->apple = appleEntity;
		//GameObject& text = createGameObject();
		//TextRenderComponent* textComp = text.addComponent<TextRenderComponent>();
		//textComp->material = resourceManager->getMaterial("text");
		//text.transform.scale.x = 1.0f;
	}

	void SnakeScene::start()
	{
		Scene::start();

		camera.rotation = glm::vec3(1.3, 0.0, 0.0);

		// Move Input
		float yaw = -camera.rotation.y;
		float pitch = camera.rotation.x;

		glm::vec3 forwardDir{
			cos(pitch) * sin(yaw),
			sin(pitch),
			-cos(pitch) * cos(yaw)
		};

		camera.position = -forwardDir * 15.0f;

		reset();
	}

	void SnakeScene::updateCameraAspect(float aspectRatio)
	{
		camera.setPerspectiveProjection(glm::radians(70.0f), aspectRatio, .1f, 100.0f);
	}

	void SnakeScene::gameOver()
	{
		reset();
	}

	void SnakeScene::reset()
	{
		// Reset snake
		SnakeHead* snakeHead = entityManager.getComponent<SnakeHead>(snakeEntity);
		snakeHead->hit = false;
		snakeHead->moveDir = glm::ivec2(0);

		// Reset positions
		entityManager.getComponent<GridTransform>(snakeEntity)->gridPosition = glm::ivec2(0);
		entityManager.getComponent<GridTransform>(appleEntity)->gridPosition = glm::ivec2(grid.width / 2, grid.height / 2);

		// Destroy segments
		for (auto& segment: snakeHead->segments)
		{
			entityManager.destroyEntity(segment);
		}
		snakeHead->segments.clear();
	}

	void SnakeScene::update(float deltaTime, float totalTime)
	{
		// Game over when the snake hits a wall or itself
		if (entityManager.getComponent<SnakeHead>(snakeEntity)->hit)
			gameOver();

		for (auto& renderObj : meshRenderSystem->renderObjects)
		{
			renderSurface(renderObj);
		}
	}
}