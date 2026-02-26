#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/GameObject.hpp"

#include <vector>

#pragma once
namespace crsp {
	class Scene {
	public:
		Scene() = default;
		void init(ResourceManager* resourceManager) {  this->resourceManager = resourceManager; }

		virtual void loadResources() = 0;
		virtual void spawnObjects() = 0;
		virtual void start();
		virtual void update(float deltaTime, float totalTime);

		std::vector<GameObject>& getGameObjects() { return gameObjects; }

	protected:
		ResourceManager* resourceManager;
		std::vector<GameObject> gameObjects{};
	};
}