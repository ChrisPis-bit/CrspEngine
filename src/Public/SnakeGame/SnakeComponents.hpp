#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ECS/Entity.hpp"

#include <vector>

namespace crsp {
	struct Grid {
		int width, height;
	};

	struct GridTransform {
		glm::ivec2 gridPosition;
	};

	struct SnakeHead {
		glm::ivec2 moveDir;
		Entity apple;
		std::vector<Entity> segments;
		bool hit;
	};
}