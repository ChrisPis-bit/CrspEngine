#include "SnakeGame/SnakeScene.hpp"
#include "SceneLogic/MeshRenderComponent.hpp"
#include "SceneLogic/TextRenderComponent.hpp"
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

	void SnakeScene::spawnObjects()
	{
		// Create gameobjects
		MeshRenderComponent* renderComp;

		GameObject& vikingRoomObject = createGameObject();
		renderComp = vikingRoomObject.addComponent<MeshRenderComponent>();
		renderComp->mesh = resourceManager->getMesh("viking_room");
		renderComp->material = resourceManager->getMaterial("standard");
		vikingRoomObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);

		GameObject& quadObject = createGameObject();
		renderComp = quadObject.addComponent<MeshRenderComponent>();
		renderComp->mesh = resourceManager->getMesh("quad");
		renderComp->material = resourceManager->getMaterial("standard");
		quadObject.transform.position = glm::vec3(1.5f, 0.0f, 0.0f);

		GameObject& groundObject = createGameObject();
		renderComp = groundObject.addComponent<MeshRenderComponent>();
		renderComp->mesh = resourceManager->getMesh("quad");
		groundObject.transform.scale = glm::vec3(5.0f, 5.0f, 5.0f);
		groundObject.transform.rotation = glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f);
		renderComp->material = resourceManager->getMaterial("standard");

		GameObject& snakeHead = createGameObject();
		renderComp = snakeHead.addComponent<MeshRenderComponent>();
		renderComp->mesh = resourceManager->getMesh("cube");
		renderComp->material = resourceManager->getMaterial("snake");
		snakeHead.transform.scale = glm::vec3(.5f);

		GameObject& apple = createGameObject();
		renderComp = apple.addComponent<MeshRenderComponent>();
		renderComp->mesh = resourceManager->getMesh("cube");
		apple.transform.scale = glm::vec3(.5f);
		renderComp->material = resourceManager->getMaterial("apple");

		GameObject& text = createGameObject();
		TextRenderComponent* textComp = text.addComponent<TextRenderComponent>();
		textComp->material = resourceManager->getMaterial("text");
		text.transform.scale.x = 1.0f;

		snakeObj = &snakeHead;
		appleObj = &apple;

		uint32_t totalGridCells = gridHeight * gridWidth;
		snakeSegments.resize(totalGridCells);
		for (size_t i = 0; i < totalGridCells; i++)
		{
			GameObject& segment = createGameObject();
			renderComp = segment.addComponent<MeshRenderComponent>();
			renderComp->mesh = resourceManager->getMesh("cube");
			segment.transform.scale = glm::vec3(.5f);
			renderComp->material = resourceManager->getMaterial("snake");
			segment.isActive = false;

			snakeSegments[i] = gameObjects.back().get();
		}
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
			segment->isActive = false;
		}

		moveDir = glm::ivec2(0);
		moveTimer = 0;
	}

	void SnakeScene::update(float deltaTime, float totalTime)
	{
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

				if (segmentPositions.size() < snakeSegments.size()) segmentPositions.push_back(lastSegmentPos);
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
			GameObject* segment = snakeSegments[i];
			segment->isActive = true;
			segment->transform.position = glm::vec3(segmentPositions[i].x - gridWidth / 2.0f, 0, -segmentPositions[i].y + gridHeight / 2.0f);
		}

		snakeObj->transform.position = glm::vec3(snakePos.x - gridWidth / 2.0f, 0, -snakePos.y + gridHeight / 2.0f);
		appleObj->transform.position = glm::vec3(applePos.x - gridWidth / 2.0f, 0, -applePos.y + gridHeight / 2.0f);
	}
}