#include "SnakeGame/Systems/GridTransformSystem.hpp"

#include <algorithm>
#include <execution> 

namespace crsp {
	void GridTransformSystem::update(float deltaTime, float currentTime)
	{
		// Parallel, since ECS easily allows for it
		std::for_each(std::execution::par, entities.begin(), entities.end(), [deltaTime, this](Entity entity) {
			Transform* transform = entityManager.getComponent<Transform>(entity);
			GridTransform* gridTransform = entityManager.getComponent<GridTransform>(entity);

			// Transforms grid positions to 3D transform.
			glm::vec3 dir = glm::vec3(gridTransform->gridPosition.x - grid.width / 2.0f + .5f, 0, -gridTransform->gridPosition.y + grid.height / 2.0f + .5f) - transform->position;
			transform->position += dir / 2.0f * deltaTime * LERP_SPEED;
		});
	}
}