#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "SnakeGame/SnakeComponents.hpp"
#include "SceneLogic/InputSystem.hpp"
#include "Rendering/ResourceManager.hpp"

#include <queue>

namespace crsp {
	/// <summary>
	/// Controls the movement of the snake and handles collision with the apple.
	/// </summary>
	class SnakeControllerSystem : public BaseSystem<Transform, GridTransform, SnakeHead> {
	public:
		constexpr static float MOVE_INTERVAL = .3f; // Interval at which the snake and segments move
		constexpr static int MAX_INPUT_QUEUE_SIZE = 2; // Maximum amount of inputs in the input queue

		SnakeControllerSystem(EntityManager& entityManager, Grid& grid, InputSystem& inputSystem, ResourceManager& resourceManager) : BaseSystem(entityManager), grid{ grid }, inputSystem{ inputSystem }, resourceManager{ resourceManager } {}
		~SnakeControllerSystem() = default;

		void update(float deltaTime, float currentTime) override;

		/// <summary>
		/// Handles creating a new snake segment entity.
		/// </summary>
		/// <param name="position">Grid position of the segment</param>
		/// <returns>Created entity</returns>
		Entity createSnakeSegment(glm::ivec2 position);

	private:
		std::queue<glm::ivec2> inputQueue{};
		float moveTimer = 0;
		Grid& grid;
		InputSystem& inputSystem;
		ResourceManager& resourceManager;
	};
}