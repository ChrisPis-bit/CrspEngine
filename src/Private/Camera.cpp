#include "Camera.hpp"

//std
#include <cassert>
#include <limits>

#include <glm/gtc/matrix_transform.hpp>

namespace crsp {

	void Camera::setOrthographicProjection(
		float left, float right, float top, float bottom, float near, float far) {
		projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
	}

	void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
		projectionMatrix = glm::perspective(fovy, aspect, near, far);
	}

	void Camera::updateViewMatrix()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, position);
		viewMatrix[3][1] = -glm::dot(v, position);
		viewMatrix[3][2] = -glm::dot(w, position);
	}
	void Camera::move(GLFWwindow* window, float deltaTime)
	{
		// Look Input
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		glm::vec2 newMousePosition = { xPos, yPos };

		auto deltaMouse = newMousePosition - prevMousePosition;
		glm::vec3 rotate{ 0 };
		rotate.y += -deltaMouse.x;
		rotate.x += deltaMouse.y;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
			rotation += lookSensitivity * glm::normalize(rotate);

		rotation.x = glm::clamp(rotation.x, -1.5f, 1.5f);
		rotation.y = glm::mod(rotation.y, glm::two_pi<float>());

		prevMousePosition = newMousePosition;

		// Move Input
		float yaw = -rotation.y;
		float pitch = rotation.x;

		glm::vec3 forwardDir{
			cos(pitch) * sin(yaw),
			sin(pitch),
			-cos(pitch) * cos(yaw)
		};		
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };
		const glm::vec3 rightDir{ -forwardDir.z, 0.0f, forwardDir.x };

		glm::vec3 moveDir{ 0.0f };
		if (glfwGetKey(window, keys.moveLeft)) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveRight)) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveUp)) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown)) moveDir -= upDir;
		if (glfwGetKey(window, keys.moveForward)) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward)) moveDir -= forwardDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
			position += moveSpeed * deltaTime * glm::normalize(moveDir);
	}
} //namespace
