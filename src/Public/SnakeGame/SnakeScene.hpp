#pragma once
#include "SceneLogic/Scene.hpp"
#include "ECS/Systems/MeshRenderSystem.hpp"
#include "ECS/Systems/TextRenderSystem.hpp"
#include "SnakeGame/Systems/SnakeControllerSystem.hpp"
#include "SnakeComponents.hpp"

namespace crsp {
	class SnakeControllerSystem;

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
		void render();
		void updateCameraAspect(float aspectRatio) override;
	private:

		void gameOver();
		void reset();

		std::shared_ptr<MeshRenderSystem> meshRenderSystem;
		std::shared_ptr<TextRenderSystem> textRenderSystem;
		std::shared_ptr<SnakeControllerSystem> snakeControllerSystem;

		Grid grid{ 9, 9 };

		Entity snakeEntity;
		Entity appleEntity;
		Entity scoreTextEntity;

		bool gameEnded;
	};
}