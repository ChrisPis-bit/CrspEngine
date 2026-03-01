#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include "Rendering/UIRenderer.hpp"
#include "Camera.hpp"

#include <vector>

#pragma once
namespace crsp {
	struct RenderData {
		std::vector<RenderObject> renderObjects{};
		std::vector<UIRenderObject> UIrenderObjects{};
	};

	/// <summary>
	/// Base scene class.
	/// Stores all necessary references for a game.
	/// </summary>
	class Scene {
	public:
		Scene() = default;
		~Scene();

		void init(ResourceManager* resourceManager, InputSystem* inputSystem) {
			this->resourceManager = resourceManager;
			this->inputSystem = inputSystem;

			loadResources();
			spawnObjects();
		}

		virtual void loadResources() = 0;
		virtual void spawnObjects() = 0;
		virtual void start() {}
		void tick(float deltaTime, float totalTime);
		virtual void update(float deltaTime, float totalTime) = 0;

		virtual void updateCameraAspect(float aspectRatio) {
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, .1f, 100.0f);
		}

		GameObject& createGameObject();

		Camera& getCamera() { return camera; }
		std::vector<std::unique_ptr<GameObject>>& getGameObjects() { return gameObjects; }
		RenderData getRenderData() { return renderData; }
		ResourceManager& getResourceManager() { return *resourceManager; }

		void renderSurface(RenderObject renderObj) {
			renderData.renderObjects.push_back(renderObj);
		}

		void renderUI(UIRenderObject UIRenderObj) {
			renderData.UIrenderObjects.push_back(UIRenderObj);
		}

	protected:

		RenderData renderData{};
		ResourceManager* resourceManager;
		InputSystem* inputSystem;

		Camera camera{};
		std::vector<std::unique_ptr<GameObject>> gameObjects{};
	};
}