#include "SnakeGame/Systems/SnakeControllerSystem.hpp"
#include "Utils.hpp"

namespace crsp {
	void SnakeControllerSystem::update(float deltaTime, float currentTime)
	{
		// Queues inputs to allow for inputs in fast succesion, such as for making quick U turns
		if (inputQueue.size() < MAX_INPUT_QUEUE_SIZE) {
			if (inputSystem.isKeyPressed(GLFW_KEY_A)) inputQueue.push(glm::ivec2(-1, 0));
			if (inputSystem.isKeyPressed(GLFW_KEY_D)) inputQueue.push(glm::ivec2(1, 0));
			if (inputSystem.isKeyPressed(GLFW_KEY_W)) inputQueue.push(glm::ivec2(0, 1));
			if (inputSystem.isKeyPressed(GLFW_KEY_S)) inputQueue.push(glm::ivec2(0, -1));
		}

		// Update on interval
		moveTimer += deltaTime;
		if (moveTimer >= MOVE_INTERVAL) {
			moveTimer = 0;

			// Get movement input
			glm::ivec2 move{ 0 };
			if (inputQueue.size() > 0) {
				move = inputQueue.front();
				inputQueue.pop();
			}

			for (auto const& entity : entities)
			{
				// Get snake components
				Transform* transform = entityManager.getComponent<Transform>(entity);
				GridTransform* gridTransform = entityManager.getComponent<GridTransform>(entity);
				SnakeHead* snakeHead = entityManager.getComponent<SnakeHead>(entity);

				// Set movement direction
				if (move != glm::ivec2(0, 0) && move != -snakeHead->moveDir)
					snakeHead->moveDir = move;

				// Get position to add new segment on, if needed
				glm::ivec2 lastSegmentPos = snakeHead->segments.size() > 0 ?
					entityManager.getComponent<GridTransform>(snakeHead->segments.back())->gridPosition : // Place in back
					gridTransform->gridPosition; // Place where snake was

				// Move each segment
				for (int i = snakeHead->segments.size() - 1; i >= 0; i--)
				{
					GridTransform* curSegment = entityManager.getComponent<GridTransform>(snakeHead->segments[i]);
					if (i - 1 >= 0) {
						GridTransform* nextSegment = entityManager.getComponent<GridTransform>(snakeHead->segments[i - 1]);
						curSegment->gridPosition = nextSegment->gridPosition;
					}
					else {
						curSegment->gridPosition = gridTransform->gridPosition;
					}
				}

				// Move the snake head
				gridTransform->gridPosition += snakeHead->moveDir;

				// Collision with apple
				GridTransform* appleTransform = entityManager.getComponent<GridTransform>(snakeHead->apple);
				if (gridTransform->gridPosition == appleTransform->gridPosition) {
					// Update apple position
					appleTransform->gridPosition = glm::ivec2(randomRange(0, grid.width + 1), randomRange(0, grid.height + 1));

					// Add new segment
					snakeHead->segments.push_back(createSnakeSegment(lastSegmentPos));
				}

				// Check collision with grid edges
				if (gridTransform->gridPosition.x < 0 || gridTransform->gridPosition.x > grid.width
					|| gridTransform->gridPosition.y < 0 || gridTransform->gridPosition.y > grid.height)
					snakeHead->hit = true;

				// Check collision with self
				for (auto& segment : snakeHead->segments)
				{
					GridTransform* segmentTransform = entityManager.getComponent<GridTransform>(segment);
					if (gridTransform->gridPosition == segmentTransform->gridPosition) {
						snakeHead->hit = true;
						break;
					}
				}
			}
		}
	}

	Entity SnakeControllerSystem::createSnakeSegment(glm::ivec2 position)
	{
		Entity newSegment = entityManager.createEntity();
		entityManager.addComponent<Transform>(newSegment)->scale = glm::vec3(.5f);
		MeshRender* meshRender = entityManager.addComponent<MeshRender>(newSegment);
		meshRender->material = resourceManager.getMaterial("snake").get();
		meshRender->mesh = resourceManager.getMesh("cube").get();
		entityManager.addComponent<GridTransform>(newSegment)->gridPosition = position;

		return newSegment;
	}
}