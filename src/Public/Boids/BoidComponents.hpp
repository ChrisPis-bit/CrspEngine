#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ECS/Entity.hpp"

struct BoidSettings {
	float separationWeight = 2.0f;
	float alignmentWeight = 1.0f;
	float cohesionWeight = 0.6f;

	float separationRadius = 1.0f;
	float alignmentRadius = 2.0f;
	float cohesionRadius = 3.0f;

	float maxRadius() {
		return std::max(separationRadius, std::max(alignmentRadius, cohesionRadius));
	}
};

struct Boid {
	glm::vec3 velocity;
};