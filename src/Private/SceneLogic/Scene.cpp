#include "SceneLogic/Scene.hpp"
#include "SceneLogic/MeshRenderComponent.hpp"
#include "SceneLogic/UIRenderComponent.hpp"
#include <Rendering/UIRenderer.hpp>

namespace crsp {
	Scene::~Scene()
	{
	}
	void Scene::tick(float deltaTime, float totalTime)
	{
		renderData.renderObjects.clear();
		renderData.UIrenderObjects.clear();

		for (auto& obj : gameObjects) {
			obj->update();
		}

		update(deltaTime, totalTime);

		for (auto& obj : gameObjects) {
			obj->postUpdate();
		}
	}

	GameObject& Scene::createGameObject()
	{
		gameObjects.push_back(std::make_unique<GameObject>(*this));
		return *gameObjects.back();
	}
}