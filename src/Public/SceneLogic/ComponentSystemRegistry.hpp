#pragma once

#include "SceneLogic/ComponentSystem.hpp"

#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

namespace crsp {
	/// <summary>
	/// Handles storing and pairing components to their respective systems.
	/// </summary>
	class ComponentSystemRegistry {
	public:
		ComponentSystemRegistry() {}

		template<class T>
		void registerSystem(std::unique_ptr<IComponentSystem> system) {
			static_assert(std::is_base_of<Component, T>::value, "T must be of type Component!");

			if (systemRegistry.find(typeid(T)) != systemRegistry.end()) {
				std::cout << "Attempting to register an already registered system!" << '\n';
			}
			else {
				systemRegistry.insert({ typeid(T), std::move(system) });
			}
		}

		template<class T>
		void registerComponent(T* component) {
			if (auto pair = systemRegistry.find(typeid(T)); pair != systemRegistry.end()) {
				pair->second->registerComponent(component);
			}
			else {
				std::cout << "No system found to register component to." << '\n';
			}
		}

		template<class T>
		IComponentSystem* getSystem() {
			if (auto pair = systemRegistry.find(typeid(T)); pair != systemRegistry.end()) {
				return pair->second.get();
			}
			else {
				return nullptr;
			}
		}

		std::unordered_map<std::type_index, std::unique_ptr<IComponentSystem>>& getSystems() { return systemRegistry; }
	private:
		std::unordered_map<std::type_index, std::unique_ptr<IComponentSystem>> systemRegistry;
	};
}