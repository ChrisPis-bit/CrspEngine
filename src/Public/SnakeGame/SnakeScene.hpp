#pragma once
#include "SceneLogic/Scene.hpp"

namespace crsp {
	class SnakeScene : public Scene {
	public:
		SnakeScene() = default;

		void loadResources() override;
		void spawnObjects() override;
		void start() override;
		void update(float deltaTime, float totalTime) override;
		void updateCameraAspect(float aspectRatio) override;
	private:

		void gameOver();
		void reset();

		uint32_t gridWidth = 12;
		uint32_t gridHeight = 12;
		float moveInterval = .5f;
		float moveTimer = 0;

		glm::ivec2 snakePos;
		glm::ivec2 moveDir;
		glm::ivec2 applePos;

		GameObject* snakeObj;
		GameObject* appleObj;

		std::vector<GameObject*> snakeSegments;
		std::vector<glm::ivec2> segmentPositions;
	};
}