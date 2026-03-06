#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ECS/Entity.hpp"

struct BoidSettings {
	float boundsMin = -100.0f;
	float boundsMax = 100.0f;

	float separationWeight = 10.0f;
	float alignmentWeight = 1.0f;
	float cohesionWeight = 0.6f;

	float separationRadius = 2.0f;
	float alignmentRadius = 3.0f;
	float cohesionRadius = 4.0f;

	float maxSpeed = 5.0f;

	float maxRadius() {
		return std::max(separationRadius, std::max(alignmentRadius, cohesionRadius));
	}
};

struct Boid {
	glm::vec3 velocity;
};