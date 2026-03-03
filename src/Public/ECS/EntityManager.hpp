#pragma once
#include "Entity.hpp"
#include "ComponentPool.hpp"
#include "SystemManager.hpp"

#include <vector>
#include <iostream>
#include <type_traits>

namespace crsp {
	class EntityManager {
	public:
		EntityManager() = default;
		~EntityManager() = default;

		struct EntityDesc {
			Entity entity;
			ComponentMask mask;
		};

		Entity createEntity();
		void destroyEntity(Entity entity);

		template<typename T, typename... Args>
		T* addComponent(Entity entity, Args&&... args) {
			int componentId = getId<T>();
			size_t componentPoolsAmount = componentPools.size();

			if (componentPoolsAmount <= componentId) {
				componentPools.resize(componentId + 1);
				componentPools[componentId] = std::make_unique<ComponentPool<T>>();
			}
			else if (!componentPools[componentId]) {
				componentPools[componentId] = std::make_unique<ComponentPool<T>>();
			}

			ComponentPool<T>* componentPool = static_cast<ComponentPool<T>*>(componentPools[componentId].get());
			T* newComponent = new (componentPool->get(entity.getIndex())) T(std::forward<Args>(args)...);

			entities[entity.getIndex()].mask.set(componentId);
			systemManager.entityComponentMaskChanged(entity, entities[entity.getIndex()].mask);

			return newComponent;
		}

		template<typename T>
		T* getComponent(Entity entity) {
			int componentId = getId<T>();
			if (!entities[entity.getIndex()].mask.test(componentId))
				return nullptr;

			ComponentPool<T>* componentPool = static_cast<ComponentPool<T>*>(componentPools[componentId].get());
			return componentPool->get(entity.getIndex());
		}

		template<typename T>
		void removeComponent(Entity entity) {
			if (entities[entity.getIndex()].entity != entity)
				return;

			int componentId = getId<T>();
			entities[entity.getIndex()].mask.reset(componentId);

			systemManager.entityComponentMaskChanged(entity, entities[entity.getIndex()].mask);
		}

		template<typename T, typename... Args>
		std::shared_ptr<T> registerSystem(Args&&... args) { return systemManager.registerSystem<T>(*this, std::forward<Args>(args)...); }

		void updateSystems(float deltaTime, float currentTime) { systemManager.updateSystems(deltaTime, currentTime); }
		void lateUpdateSystems(float deltaTime, float currentTime) { systemManager.lateUpdateSystems(deltaTime, currentTime); }

	private:
		inline Entity createEntityId(EntityIndex index, EntityVersion version);

		SystemManager systemManager{};

		std::vector<EntityDesc> entities;
		std::vector<std::unique_ptr<IComponentPool>> componentPools;
		std::vector<EntityIndex> freeEntities;
	};
}