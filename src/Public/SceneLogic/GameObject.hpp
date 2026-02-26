#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include "SceneLogic/Component.hpp"

#include <memory>
#include <vector>
#include <utility>

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

		GameObject() = default;

		virtual void start() {}
		virtual void update() {}

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args);

		template<typename T>
		bool FindComponent(T* component);

		RenderObject renderData();

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		Transform transform{};

	private:
		std::vector<std::unique_ptr<Component>> components{};
	};
} // namespace
