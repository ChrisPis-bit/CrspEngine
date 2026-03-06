#include "Boids/Systems/BoidSystem.hpp"

#include <algorithm>
#include <execution> 

namespace crsp {
	void BoidSystem::update(float deltaTime, float currentTime)
	{
		for (auto const& entity : entities)
		{
			Transform* transform = entityManager.getComponent<Transform>(entity);
			Boid* boid = entityManager.getComponent<Boid>(entity);

			int alignmentNeighbours = 0;
			glm::vec3 alignment(0.0f);

			int cohesionNeighbours = 0;
			glm::vec3 center(0.0f);

			glm::vec3 seperation(0.0f);

			for (auto const& otherEntity : entities)
			{
				// Skip self
				if (otherEntity == entity)
					continue;

				Transform* otherTransform = entityManager.getComponent<Transform>(otherEntity);
				Boid* otherBoid = entityManager.getComponent<Boid>(otherEntity);

				// Calculate dist and direction
				glm::vec3 distVec = transform->position - otherTransform->position;
				float dist = glm::length(distVec);
				glm::vec3 dir = distVec / dist;

				// Alignment
				if (dist <= settings.alignmentRadius) {
					alignmentNeighbours++;
					alignment += otherBoid->velocity;
				}

				// Cohesion
				if (dist <= settings.cohesionRadius) {
					cohesionNeighbours++;
					center += otherTransform->position;
				}

				// Separation
				if (dist <= settings.separationRadius) {
					seperation += dir * (settings.separationRadius - dist);
				}
			}

			// Alignment
			if (alignmentNeighbours > 0) {
				alignment /= alignmentNeighbours;
				boid->velocity += alignment * settings.alignmentWeight * deltaTime;
			}

			// Cohesion
			if (cohesionNeighbours > 0) {
				center /= cohesionNeighbours;
				glm::vec3 cohesionDir = glm::normalize(center - transform->position);
				boid->velocity += cohesionDir * settings.cohesionWeight * deltaTime;
			}

			boid->velocity += seperation * settings.separationWeight * deltaTime;

			// Clamp to max velocity
			if (glm::length(boid->velocity) > settings.maxSpeed) boid->velocity = glm::normalize(boid->velocity) * settings.maxSpeed;

			// Apply velocity
			transform->position += boid->velocity * deltaTime;

			// Clamp positions to bounds, and bounce velocity
			if (transform->position.x <= settings.boundsMin) {
				transform->position.x = settings.boundsMin;
				boid->velocity.x *= -1;
			}
			if (transform->position.x >= settings.boundsMax) {
				transform->position.x = settings.boundsMax;
				boid->velocity.x *= -1;
			}

			if (transform->position.y <= settings.boundsMin) {
				transform->position.y = settings.boundsMin;
				boid->velocity.y *= -1;
			}
			if (transform->position.y >= settings.boundsMax) {
				transform->position.y = settings.boundsMax;
				boid->velocity.y *= -1;
			}

			if (transform->position.z <= settings.boundsMin) {
				transform->position.z = settings.boundsMin;
				boid->velocity.z *= -1;
			}
			if (transform->position.z >= settings.boundsMax) {
				transform->position.z = settings.boundsMax;
				boid->velocity.z *= -1;
			}

			// Rotate
			glm::vec3 velNormalized = glm::normalize(boid->velocity);
			transform->rotation = glm::vec3(std::asin(-velNormalized.y), std::atan2(velNormalized.x, velNormalized.z), 0.0f);
		}
	}
}