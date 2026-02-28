#include "SceneLogic/Scene.hpp"
#include "SceneLogic/MeshRenderComponent.hpp"

namespace crsp {
	void Scene::start() {
		for (auto& obj : gameObjects) {
			obj->start();
		}
	}

	void Scene::update(float deltaTime, float totalTime) {
		for (auto& system : componentSystemRegistry.getSystems())
		{
			system.second->update();
		}

		for (auto& obj : gameObjects) {
			obj->update();
		}

		for (auto& system : componentSystemRegistry.getSystems())
		{
			system.second->lateUpdate();
		}
	}

	void Scene::createComponentSystems()
	{
		// Register mesh rendering component system.
		componentSystemRegistry.registerSystem<MeshRenderComponent>(std::make_unique<MeshRenderComponentSystem>());
	}

	std::vector<RenderObject> Scene::getRenderData()
	{
		// Retrieves render data from the render system, if it exists.
		if (auto renderSystem = componentSystemRegistry.getSystem<MeshRenderComponent>()) {
			return static_cast<MeshRenderComponentSystem*>(renderSystem)->copyRenderQueue();
		}
		else
			return std::vector<RenderObject>();
	}

	GameObject& Scene::createGameObject()
	{
		gameObjects.push_back(std::make_unique<GameObject>(*this));
		return *gameObjects.back();
	}
}