#include "Boids/Systems/BoidSystem.hpp"

namespace crsp {
	void BoidSystem::update(float deltaTime, float currentTime)
	{
		for (auto const& entity : entities)
		{
			Transform* transform = entityManager.getComponent<Transform>(entity);
			Boid* boid = entityManager.getComponent<Boid>(entity);

			
		}
	}
}