#include "SnakeGame/Systems/GridTransformSystem.hpp"

namespace crsp {
	void GridTransformSystem::update(float deltaTime, float currentTime)
	{
		for (auto const& entity : entities)
		{
			Transform* transform = entityManager.getComponent<Transform>(entity);
			GridTransform* gridTransform = entityManager.getComponent<GridTransform>(entity);

			// Transforms grid positions to 3D transform.
			transform->position = glm::vec3(gridTransform->gridPosition.x - grid.width / 2.0f, 0, -gridTransform->gridPosition.y + grid.height / 2.0f);
		}
	}
}