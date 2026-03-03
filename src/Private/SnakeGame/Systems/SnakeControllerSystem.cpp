#include "SnakeGame/Systems/SnakeControllerSystem.hpp"
#include "Utils.hpp"

#include <format>
#include <cmath>

#include <glm/gtc/constants.hpp>;

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
				if (move != glm::ivec2(0, 0) && move != -snakeHead->moveDir) {
					snakeHead->moveDir = move;

					// Rotate head
					transform->rotation.y = glm::pi<float>() - std::atan2(snakeHead->moveDir.x, snakeHead->moveDir.y);
				}

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
					appleTransform->gridPosition = getFreePosition(snakeHead->segments, gridTransform->gridPosition);

					// Add new segment
					snakeHead->segments.push_back(createSnakeSegment(lastSegmentPos));

					// Update score
					entityManager.getComponent<TextRender>(snakeHead->scoreText)->set(SnakeHead::SCORE_TEXT + std::to_string(snakeHead->segments.size()));
				}

				// Check collision with grid edges
				if (gridTransform->gridPosition.x < 0 || gridTransform->gridPosition.x >= grid.width
					|| gridTransform->gridPosition.y < 0 || gridTransform->gridPosition.y >= grid.height)
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
		meshRender->material = resourceManager.getMaterial("snake_material");
		meshRender->mesh = resourceManager.getMesh("snake_body");
		entityManager.addComponent<GridTransform>(newSegment)->gridPosition = position;

		return newSegment;
	}

	glm::ivec2 SnakeControllerSystem::getFreePosition(std::vector<Entity> segments, glm::ivec2 head)
	{
		std::vector<bool> availableCells;
		availableCells.resize(grid.width * grid.height, true);

		// Occupy snake head
		if (grid.inGrid(head))
			availableCells[head.y * grid.width + head.x] = false;

		// Occupy segment cells
		for (auto& segment : segments)
		{
			glm::ivec2 gridPos = entityManager.getComponent<GridTransform>(segment)->gridPosition;
			if (grid.inGrid(gridPos))
				availableCells[gridPos.y * grid.width + gridPos.x] = false;
		}

		// Construct list of possible positions
		std::vector<glm::ivec2> possiblePositions;
		for (size_t i = 0; i < availableCells.size(); i++)
		{
			if (!availableCells[i]) continue;

			int x = i % grid.width;
			int y = i / grid.width;
			possiblePositions.push_back(glm::ivec2(x, y));
		}

		// Return random possible position
		return possiblePositions[randomRange(0, possiblePositions.size())];
	}
}