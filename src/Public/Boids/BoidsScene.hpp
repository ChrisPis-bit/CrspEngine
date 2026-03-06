#pragma once
#include "SceneLogic/Scene.hpp"
#include "ECS/Systems/MeshRenderSystem.hpp"
#include "ECS/Systems/TextRenderSystem.hpp"
#include "Boids/BoidComponents.hpp"

namespace crsp {
	class BoidsScene : public Scene {
	public:
		~BoidsScene() = default;

		void loadResources() override;
		void registerSystems() override;
		void spawnEntities() override;
		void start() override;
		void update(float deltaTime, float totalTime) override;
		void render() override;

		void updateCameraAspect(float aspectRatio) override {
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, .1f, 500.0f);
		}
	private:
		std::shared_ptr<MeshRenderSystem> meshRenderSystem;
		std::shared_ptr<TextRenderSystem> textRenderSystem;

		BoidSettings boidSettings{};
	};
}