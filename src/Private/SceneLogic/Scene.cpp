#include "SceneLogic/Scene.hpp"

namespace crsp {
	void Scene::start() {
		for (auto& obj : gameObjects) {
			obj.start();
		}
	}

	void Scene::update(float deltaTime, float totalTime) {
		for (auto& obj : gameObjects) {
			obj.update();
		}
	}
}