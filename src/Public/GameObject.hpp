#pragma once

#include "glm/glm.hpp"

#include "Rendering/Mesh.hpp"

#include <memory>

namespace crsp {
	struct Transform {
		glm::vec3 position;
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation;

		glm::mat4 calculateTransformationMatrix();
		glm::mat4 calculateNormalMatrix();
	};

	class GameObject {
	public:
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		std::shared_ptr<Mesh> mesh;
		Transform transform{};
	};
} // namespace
