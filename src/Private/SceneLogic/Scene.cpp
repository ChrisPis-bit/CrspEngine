#include "SceneLogic/Scene.hpp"
#include <Rendering/UIRenderer.hpp>

namespace crsp {
	Scene::~Scene()
	{
	}
	void Scene::tick(float deltaTime, float totalTime)
	{
		renderData.renderObjects.clear();
		renderData.UIrenderObjects.clear();

		entityManager.updateSystems(deltaTime, totalTime);

		update(deltaTime, totalTime);
	}
}