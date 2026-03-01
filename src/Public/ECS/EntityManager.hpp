#pragma once
#include "Entity.hpp"
#include "ComponentPool.hpp"
#include "SystemManager.hpp"

#include <vector>
#include <iostream>

namespace crsp {
	class EntityManager {
	public:
		struct EntityDesc {
			Entity entity;
			ComponentMask mask;
		};

		Entity createEntity();
		void destroyEntity(Entity entity);

		template<typename T>
		T* addComponent(Entity entity) {
			int componentId = getId<T>();
			size_t componentPoolsAmount = componentPools.size();

			if (componentPoolsAmount <= componentId) {
				componentPools.resize(componentPoolsAmount + 1);
				componentPools[componentId] = new ComponentPool(sizeof(T));
			}
			T* newComponent = new (componentPools[componentId]->get(entity.getIndex())) T();

			entities[entity.getIndex()].mask.set(componentId);
			systemManager.entityComponentMaskChanged(entity, entities[entity.getIndex()].mask);

			return newComponent;
		}

		template<typename T>
		T* getComponent(Entity entity) {
			int componentId = getId<T>();
			if (!entities[entity.getIndex()].mask.test(componentId))
				return nullptr;

			return static_cast<T*>(componentPools[componentId]->get(entity.getIndex()));
		}

		template<typename T>
		void removeComponent(Entity entity) {
			if (entities[entity.getIndex()].entity != entity)
				return;

			int componentId = getId<T>();
			entities[entity.getIndex()].mask.reset(componentId);

			systemManager.entityComponentMaskChanged(entity, entities[entity.getIndex()].mask);
		}

		template<typename T>
		std::shared_ptr<T> registerSystem() { return systemManager.registerSystem<T>(*this); }

		void updateSystems(float deltaTime, float currentTime) { systemManager.updateSystems(deltaTime, currentTime); }

	private:
		inline Entity createEntityId(EntityIndex index, EntityVersion version);

		SystemManager systemManager{};

		std::vector<EntityDesc> entities;
		std::vector<ComponentPool*> componentPools;
		std::vector<EntityIndex> freeEntities;
	};
}