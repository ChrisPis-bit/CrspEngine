#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "ECS/Entity.hpp"

#include <vector>

namespace crsp {
	struct Grid {
		int width, height;

		bool inGrid(glm::ivec2 pos) {
			return pos.x < width && pos.x >= 0 && pos.y < height && pos.x >= 0;
		}
	};

	struct GridTransform {
		glm::ivec2 gridPosition;
	};

	struct SnakeHead {
		constexpr static const char* SCORE_TEXT = "Score: ";

		SnakeHead() {}
		SnakeHead(Entity apple, Entity scoreText) : apple(apple), scoreText(scoreText) {}

		glm::ivec2 moveDir;
		Entity apple;
		Entity scoreText;
		std::vector<Entity> segments;
		bool hit;
	};
}