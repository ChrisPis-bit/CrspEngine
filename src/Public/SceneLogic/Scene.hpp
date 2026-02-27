#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "Camera.hpp"

#include <vector>

#pragma once
namespace crsp {
	class Scene {
	public:
		Scene() = default;
		void init(ResourceManager* resourceManager, InputSystem* inputSystem) {
			this->resourceManager = resourceManager;
			this->inputSystem = inputSystem;
		}

		virtual void loadResources() = 0;
		virtual void spawnObjects() = 0;
		virtual void start();
		virtual void update(float deltaTime, float totalTime);
		virtual void updateCameraAspect(float aspectRatio) {
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, .1f, 100.0f);
		}

		std::vector<std::unique_ptr<GameObject>>& getGameObjects() { return gameObjects; }
		Camera& getCamera() { return camera; }

	protected:
		ResourceManager* resourceManager;
		InputSystem* inputSystem;

		std::vector<std::unique_ptr<GameObject>> gameObjects{};

		Camera camera{};
	};
}