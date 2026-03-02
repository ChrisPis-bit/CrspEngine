#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "SnakeGame/SnakeComponents.hpp"

namespace crsp {
	/// <summary>
	/// Handles translating the GridTransform to the normal 3D Transform.
	/// </summary>
	class GridTransformSystem : public BaseSystem<Transform, GridTransform> {
	public:
		GridTransformSystem(EntityManager& entityManager, Grid& grid) : BaseSystem(entityManager), grid{ grid } {}
		~GridTransformSystem() = default;

		void update(float deltaTime, float currentTime) override;

	private:
		Grid& grid;
	};
}