#include "SnakeGame/SnakeScene.hpp"
#include "ECS/Systems/MeshRenderSystem.hpp"
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

		// register systems
		meshRenderSystem = entityManager.registerSystem<MeshRenderSystem>();		
	}

	void SnakeScene::spawnObjects()
	{
		//for (size_t i = 0; i < 10; i++)
		//{
		//	Entity entity = entityManager.createEntity();
		//	Transform* transform = entityManager.addComponent<Transform>(entity);
		//	transform->position = glm::vec3(i * 2.6f, 0.0f, 0.0f);
		//	MeshRender* meshRender = entityManager.addComponent<MeshRender>(entity);
		//	meshRender->material = resourceManager->getMaterial("apple");
		//	meshRender->mesh = resourceManager->getMesh("cube");
		//}

		snakeObj = entityManager.createEntity();
		Transform* transform = entityManager.addComponent<Transform>(snakeObj);
		transform->scale = glm::vec3(.5f);
		MeshRender* meshRender = entityManager.addComponent<MeshRender>(snakeObj);
		meshRender->material = resourceManager->getMaterial("snake");
		meshRender->mesh = resourceManager->getMesh("cube");

		appleObj = entityManager.createEntity();
		transform = entityManager.addComponent<Transform>(appleObj);
		transform->scale = glm::vec3(.5f);
		meshRender = entityManager.addComponent<MeshRender>(appleObj);
		meshRender->material = resourceManager->getMaterial("apple");
		meshRender->mesh = resourceManager->getMesh("cube");


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
		snakePos = glm::ivec2(0);
		applePos = glm::ivec2(gridWidth / 2, gridHeight / 2);

		segmentPositions.clear();

		for (auto& segment: snakeSegments)
		{
			entityManager.destroyEntity(segment);
		}
		snakeSegments.clear();

		moveDir = glm::ivec2(0);
		moveTimer = 0;
	}

	void SnakeScene::update(float deltaTime, float totalTime)
	{
		for (auto& renderObj : meshRenderSystem->renderObjects)
		{
			renderData.renderObjects.push_back(renderObj);

		}

		glm::ivec2 move = glm::ivec2(0, 0);
		if (inputSystem->isKeyPressed(GLFW_KEY_A) && moveDir != glm::ivec2(-1, 0)) move = glm::ivec2(-1, 0);
		if (inputSystem->isKeyPressed(GLFW_KEY_D) && moveDir != glm::ivec2(1, 0)) move = glm::ivec2(1, 0);
		if (inputSystem->isKeyPressed(GLFW_KEY_W) && moveDir != glm::ivec2(0, 1)) move = glm::ivec2(0, 1);
		if (inputSystem->isKeyPressed(GLFW_KEY_S) && moveDir != glm::ivec2(0, -1)) move = glm::ivec2(0, -1);

		if (move != glm::ivec2(0, 0) && move != -moveDir)
			moveDir = move;

		moveTimer += deltaTime;
		if (moveTimer >= moveInterval) {
			moveTimer = 0;


			glm::ivec2 lastSegmentPos = segmentPositions.size() > 0 ? segmentPositions.back() : snakePos;

			// move segments
			for (int i = segmentPositions.size() - 1; i >= 0; i--)
			{
				if (i - 1 >= 0) {
					segmentPositions[i] = segmentPositions[i - 1];
				}
				else {
					segmentPositions[i] = snakePos;
				}
			}

			snakePos += moveDir;

			if (snakePos == applePos) {
				applePos = glm::ivec2(randomRange(0, gridWidth + 1), randomRange(0, gridHeight + 1));

				segmentPositions.push_back(lastSegmentPos);
				Entity newSegment = entityManager.createEntity();
				entityManager.addComponent<Transform>(newSegment)->scale = glm::vec3(.5f);
				MeshRender* meshRender = entityManager.addComponent<MeshRender>(newSegment);
				meshRender->material = resourceManager->getMaterial("snake");
				meshRender->mesh = resourceManager->getMesh("cube");
				snakeSegments.push_back(newSegment);
			}

			// Check collision with grid edges
			if (snakePos.x < 0 || snakePos.x > gridWidth
				|| snakePos.y < 0 || snakePos.y > gridHeight)
				gameOver();

			// Check collision with self
			for (auto& segmentPosition : segmentPositions)
			{
				if(snakePos == segmentPosition){
					gameOver();
					break;
				}
			}
		}

		for (size_t i = 0; i < segmentPositions.size(); i++)
		{
			Transform* transform = entityManager.getComponent<Transform>(snakeSegments[i]);
			transform->position = glm::vec3(segmentPositions[i].x - gridWidth / 2.0f, 0, -segmentPositions[i].y + gridHeight / 2.0f);
		}

		Transform* transform = entityManager.getComponent<Transform>(snakeObj);
		transform->position = glm::vec3(snakePos.x - gridWidth / 2.0f, 0, -snakePos.y + gridHeight / 2.0f);

		transform = entityManager.getComponent<Transform>(appleObj);
		transform->position = glm::vec3(applePos.x - gridWidth / 2.0f, 0, -applePos.y + gridHeight / 2.0f);
	}
}