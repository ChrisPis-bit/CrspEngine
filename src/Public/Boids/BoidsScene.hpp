#pragma once
#include "SceneLogic/Scene.hpp"

namespace crsp {
	class BoidsScene : public Scene {
		~BoidsScene() = default;

		void loadResources() override;
		void registerSystems() override;
		void spawnEntities() override;
		void start() override;
		void update(float deltaTime, float totalTime) override;
		void render() override;
		void updateCameraAspect(float aspectRatio) override;
	};
}