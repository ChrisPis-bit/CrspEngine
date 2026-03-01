#include "ECS/EntityManager.hpp"

namespace crsp {
	Entity EntityManager::createEntity()
	{
		if (!freeEntities.empty()) {
			EntityIndex newIndex = freeEntities.back();
			freeEntities.pop_back();
			entities[newIndex].entity = createEntityId(newIndex, entities[newIndex].entity.getVersion());
			return entities[newIndex].entity;
		}

		entities.push_back({ createEntityId(EntityIndex(entities.size()), 0), ComponentMask()});

		auto entityDesc = entities.back();
		systemManager.entityComponentMaskChanged(entityDesc.entity, entityDesc.mask);

		return entityDesc.entity;
	}

	void EntityManager::destroyEntity(Entity entity)
	{
		Entity newInvalidEntity = createEntityId(EntityIndex(-1), entity.getVersion() + 1);
		entities[entity.getIndex()].entity = newInvalidEntity;
		entities[entity.getIndex()].mask.reset();
		freeEntities.push_back(entity.getIndex());
		systemManager.entityDestroyed(entity);
	}

	inline Entity EntityManager::createEntityId(EntityIndex index, EntityVersion version)
	{
		Entity entity{};
		entity.id = ((EntityID)index << 32) | ((EntityID)version);
		return entity;
	}
}