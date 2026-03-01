#pragma once
#include <set>

#include "ECS/Entity.hpp"
#include "ECS/EntityManager.hpp"

namespace crsp {
	class System {
	public:
		System(EntityManager& entityManager) : entityManager(entityManager) {}
		virtual ~System() = default;

		virtual void update(float deltaTime, float currentTime) {}

		void registerEntity(Entity entity) { entities.insert(entity); }
		void removeEntity(Entity entity) { entities.erase(entity); }

		inline ComponentMask getMask() { return componentMask; }
		inline bool iterateAll() { return all; }

	protected:
		std::set<Entity> entities;
		ComponentMask componentMask;
		EntityManager& entityManager;
		bool all{ false };
	};

	template<typename... ComponentTypes>
	class BaseSystem : public System {
	public:
		BaseSystem(EntityManager& entityManager) : System(entityManager) {
			if (sizeof...(ComponentTypes) == 0) {
				all = true;
			}
			else {
				int componentIds[] = { 0, getId<ComponentTypes>()... };
				for (int i = 1; i < (sizeof...(ComponentTypes) + 1); i++)
				{
					componentMask.set(componentIds[i]);
				}
			}
		}
	};
}