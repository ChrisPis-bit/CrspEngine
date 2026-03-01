#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "SceneLogic/Component.hpp"

#include <memory>
#include <vector>
#include <utility>

namespace crsp {

	struct Transform {
		glm::vec3 position{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		glm::mat4 calculateTransformationMatrix();
		glm::mat4 calculateNormalMatrix();

		glm::mat3 calculateTransformationMatrix2D();
	};

	class Scene;

	class GameObject {
	public:
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		GameObject(Scene& scene) : scene(scene) {}
		virtual ~GameObject() = default;

		virtual void update() {}
		virtual void postUpdate();

		/// <summary>
		/// Add component of type.
		/// Handles registering component to relevant system.
		/// </summary>
		/// <typeparam name="T">Component type</typeparam>
		/// <typeparam name="...Args">Component constructor argument types</typeparam>
		/// <param name="...args">Component constructor arguments</param>
		/// <returns>Added component</returns>
		template<typename T, typename... Args>
		T* addComponent(Args&&... args) {
			static_assert(std::is_base_of<Component, T>::value, "T must be of type Component!");

			auto Ptr = std::make_unique<T>(*this, std::forward<Args>(args)...);
			T* RawPtr = Ptr.get();
			components.push_back(std::move(Ptr));

			return RawPtr;
		}

		/// <summary>
		/// Gets component of type.
		/// Returns nullptr if it doesn't exist.
		/// </summary>
		/// <typeparam name="T">Component type</typeparam>
		/// <returns>Found component</returns>
		template<typename T>
		T* getComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T must be of type Component!");

			for (auto comp : components)
			{
				if (auto ptr = dynamic_cast<T*>(comp.get())) {
					return ptr;
				}
			}
			return false;
		}

		Scene& getScene() { return scene; }

		Transform transform{};
		bool isActive = true;

	protected:
		Scene& scene;

	private:
		std::vector<std::unique_ptr<Component>> components{};
	};
} // namespace
