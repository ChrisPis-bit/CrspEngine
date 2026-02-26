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
	};
}