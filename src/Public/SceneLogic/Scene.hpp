#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "SceneLogic/ComponentSystemRegistry.hpp"
#include "Camera.hpp"

#include <vector>

#pragma once
namespace crsp {
	/// <summary>
	/// Base scene class.
	/// Stores all necessary references for a game.
	/// </summary>
	class Scene {
	public:
		Scene() = default;

		void init(ResourceManager* resourceManager, InputSystem* inputSystem) {
			this->resourceManager = resourceManager;
			this->inputSystem = inputSystem;

			loadResources();
			createComponentSystems();
			spawnObjects();
		}

		virtual void loadResources() = 0;
		virtual void createComponentSystems();
		virtual void spawnObjects() = 0;
		virtual void start();
		virtual void update(float deltaTime, float totalTime);

		virtual void updateCameraAspect(float aspectRatio) {
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, .1f, 100.0f);
		}

		GameObject& createGameObject();

		Camera& getCamera() { return camera; }
		ComponentSystemRegistry& getComponentSystemRegistry() { return componentSystemRegistry; }
		std::vector<std::unique_ptr<GameObject>>& getGameObjects() { return gameObjects; }
		std::vector<RenderObject> getRenderData();

	protected:

		ResourceManager* resourceManager;
		InputSystem* inputSystem;

		Camera camera{};
		ComponentSystemRegistry componentSystemRegistry{};
		std::vector<std::unique_ptr<GameObject>> gameObjects{};
	};
}