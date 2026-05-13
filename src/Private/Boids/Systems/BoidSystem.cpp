#include "Boids/Systems/BoidSystem.hpp"

#include <algorithm>
#include <execution> 
#include <omp.h>

namespace crsp {
	void BoidSystem::update(float deltaTime, float currentTime)
	{
		std::vector<Entity const*> entityVector;
		entityVector.reserve(entities.size());

		for (auto const& e : entities)
		{
			entityVector.push_back(&e);
		}

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> velocities;

		positions.reserve(entityVector.size());
		velocities.reserve(entityVector.size());

		for (Entity const* e : entityVector)
		{
			positions.push_back(
				entityManager.getComponent<Transform>(*e)->position
			);

			velocities.push_back(
				entityManager.getComponent<Boid>(*e)->velocity
			);
		}

#pragma omp parallel for
		for (int i = 0; i < (int)entityVector.size(); i++)
		{
			Entity const& entity = *entityVector[i];
			Transform* transform = entityManager.getComponent<Transform>(entity);
			Boid* boid = entityManager.getComponent<Boid>(entity);

			int alignmentNeighbours = 0;
			glm::vec3 alignment(0.0f);

			int cohesionNeighbours = 0;
			glm::vec3 center(0.0f);

			glm::vec3 seperation(0.0f);

			for (int j = 0; j < (int)entityVector.size(); j++)
			{
				Entity const& otherEntity = *entityVector[j];

				// Skip self
				if (otherEntity == entity)
					continue;

				glm::vec3 otherBoidPos =  positions[j];
				glm::vec3 otherBoidVel = velocities[j];

				// Calculate dist and direction
				glm::vec3 distVec = transform->position - otherBoidPos;
				float dist = glm::length(distVec);
				glm::vec3 dir = distVec / dist;

				// Alignment
				if (dist <= settings.alignmentRadius) {
					alignmentNeighbours++;
					alignment += otherBoidVel;
				}

				// Cohesion
				if (dist <= settings.cohesionRadius) {
					cohesionNeighbours++;
					center += otherBoidPos;
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