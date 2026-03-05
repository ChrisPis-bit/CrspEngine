#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace crsp {
	class DirectionalLight {
	public:
		DirectionalLight();

		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

		glm::mat4 getProjectionViewMatrix();

		glm::vec3 getDirection() {
			return glm::vec3(viewMatrix[0][2],
				viewMatrix[1][2],
				viewMatrix[2][2]);
		}

	private:

		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{1.0f};
	};
}