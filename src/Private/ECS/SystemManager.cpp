#include "ECS/SystemManager.hpp"
#include "ECS/System.hpp"

namespace crsp {
	void SystemManager::entityDestroyed(Entity entity)
	{
		for (auto& pair : systems)
		{
			pair.second->removeEntity(entity);
		}
	}
	void SystemManager::entityComponentMaskChanged(Entity entity, ComponentMask mask)
	{
		for (auto& pair : systems)
		{
			auto& system = pair.second;
			if (system->iterateAll() || system->getMask() == (system->getMask() & mask)) {
				system->registerEntity(entity);
			}
			else {
				system->removeEntity(entity);
			}
		}
	}
	void SystemManager::updateSystems(float deltaTime, float currentTime)
	{
		for (auto& pair : systems)
		{
			auto& system = pair.second;
			system->update(deltaTime, currentTime);
		}
	}
}