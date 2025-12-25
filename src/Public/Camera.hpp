#pragma once

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Rendering/Window.hpp"

namespace crsp {
	class Camera {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_SPACE;
			int moveDown = GLFW_KEY_LEFT_CONTROL;
		};

		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		void updateViewMatrix();

		void move(GLFWwindow* window, float deltaTime);

		const glm::mat4& getProjection() const { return projectionMatrix; }
		const glm::mat4& getView() const { return viewMatrix; }

	private:
		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };

		glm::vec3 position;
		glm::vec3 rotation;

		KeyMappings keys{};
		float moveSpeed = 3.0f;
		float lookSensitivity = 0.01f;
		glm::vec2 prevMousePosition;
	};
}