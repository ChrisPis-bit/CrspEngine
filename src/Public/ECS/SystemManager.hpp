#pragma once
#include <set>
#include <unordered_map>
#include <typeindex>
#include <memory>

#include "ECS/Entity.hpp"

namespace crsp {
	class System;
	class EntityManager;

	class SystemManager {
	public:
		template<typename T>
		std::shared_ptr<T> registerSystem(EntityManager& entityManager){
			std::type_index typeId = typeid(T);
			assert(systems.find(typeId) == systems.end() && "Registering system more than once.");

			std::shared_ptr<T> system = std::make_shared<T>(entityManager);
			systems.insert({typeId, system });
			return system;
		}

		void entityDestroyed(Entity entity);
		void entityComponentMaskChanged(Entity entity, ComponentMask mask);
		void updateSystems(float deltaTime, float currentTime);

	private:
		std::unordered_map<std::type_index, std::shared_ptr<System>> systems{};
	};
}