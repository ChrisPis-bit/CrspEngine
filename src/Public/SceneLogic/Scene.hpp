#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include "Rendering/UIRenderer.hpp"
#include "ECS/EntityManager.hpp"
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
		virtual ~Scene() = default;

		void init(ResourceManager* resourceManager, InputSystem* inputSystem) {
			this->resourceManager = resourceManager;
			this->inputSystem = inputSystem;

			loadResources();
			registerSystems();
			spawnEntities();
		}

		void tick(float deltaTime, float totalTime);

		virtual void loadResources() = 0;
		virtual void registerSystems() = 0;
		virtual void spawnEntities() = 0;
		virtual void start() {}
		virtual void update(float deltaTime, float totalTime) {};

		virtual void updateCameraAspect(float aspectRatio) {
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, .1f, 100.0f);
		}

		Camera& getCamera() { return camera; }
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

		EntityManager entityManager;
	};
}