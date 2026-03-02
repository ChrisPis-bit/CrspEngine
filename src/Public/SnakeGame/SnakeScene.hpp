#pragma once
#include "SceneLogic/Scene.hpp"
#include "ECS/Systems/MeshRenderSystem.hpp"
#include "SnakeComponents.hpp"

namespace crsp {
	/// <summary>
	/// Example scene showcasing a basic implementation of the game Snake.
	/// </summary>
	class SnakeScene : public Scene {
	public:
		~SnakeScene() = default;

		void loadResources() override;
		void registerSystems() override;
		void spawnEntities() override;
		void start() override;
		void update(float deltaTime, float totalTime) override;
		void updateCameraAspect(float aspectRatio) override;
	private:

		void gameOver();
		void reset();

		std::shared_ptr<MeshRenderSystem> meshRenderSystem;

		Grid grid{ 12, 12 };

		Entity snakeEntity;
		Entity appleEntity;
	};
}