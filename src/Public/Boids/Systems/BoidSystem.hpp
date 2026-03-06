#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "Boids/BoidComponents.hpp"

namespace crsp {
	/// <summary>
	/// Controls boids.
	/// </summary>
	class BoidSystem : public BaseSystem<Transform, Boid> {
	public:
		BoidSystem(EntityManager& entityManager, BoidSettings& settings) : BaseSystem(entityManager), settings{ settings } {}
		~BoidSystem() = default;

		void update(float deltaTime, float currentTime) override;

	private:
		BoidSettings& settings;
	};
}