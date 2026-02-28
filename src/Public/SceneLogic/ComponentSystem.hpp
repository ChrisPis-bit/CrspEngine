#pragma once

#include "SceneLogic/Component.hpp"

#include <vector>

namespace crsp {
	class IComponentSystem {
	public:
		IComponentSystem() = default;

		virtual void registerComponent(Component* component) = 0;
		virtual void update() {}
		virtual void lateUpdate() {}
	};

	/// <summary>
	/// References components of certain type. Allows for fast lookups of set type of component. 
	/// Usefull for retrieving rendering data, or handling collision between collider components.
	/// Allows for ECS-like behaviors.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	template<class T>
	class ComponentSystem : public IComponentSystem {
	public:
		void registerComponent(Component* component) override {
			components.push_back(static_cast<T*>(component));
		}
	protected:
		std::vector<T*> components;
	};
}