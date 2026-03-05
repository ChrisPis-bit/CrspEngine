#include "Rendering/ResourceManager.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "Rendering/SurfaceRenderer.hpp"
#include "Rendering/UIRenderer.hpp"
#include "ECS/EntityManager.hpp"
#include "Camera.hpp"
#include "Core/Rendering/DirectionalLight.hpp"

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

		void init(ResourceManager* resourceManager, InputSystem* inputSystem, Window* window) {
			this->resourceManager = resourceManager;
			this->inputSystem = inputSystem;
			this->window = window;

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
		virtual void render() {};

		virtual void updateCameraAspect(float aspectRatio) {
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, .1f, 100.0f);
		}

		Camera& getCamera() { return camera; }
		DirectionalLight& getMainLight() { return mainLight; }
		RenderData& getRenderData() { return renderData; }
		ResourceManager& getResourceManager() { return *resourceManager; }
		Window& getWindow() { return *window; }
		InputSystem& getInputSystem() { return *inputSystem; }

		void renderSurfaces(std::vector<RenderObject> renderObjects) {
			renderData.renderObjects.insert(renderData.renderObjects.end(), renderObjects.begin(), renderObjects.end());
		}

		void renderUI(std::vector<UIRenderObject> UIRenderObjects) {
			renderData.UIrenderObjects.insert(renderData.UIrenderObjects.end(), UIRenderObjects.begin(), UIRenderObjects.end());
		}

	protected:

		RenderData renderData{};
		ResourceManager* resourceManager;
		InputSystem* inputSystem;
		Window* window;

		Camera camera{};
		DirectionalLight mainLight{};

		EntityManager entityManager;
	};
}