#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "Rendering/MaterialRenderer.hpp"

namespace crsp {
	class MeshRenderSystem : public BaseSystem<Transform, MeshRender>{
	public:
		MeshRenderSystem(EntityManager& entityManager) : BaseSystem(entityManager) {}

		void update(float deltaTime, float currentTime) {
			renderObjects.clear();
			for (auto const& entity: entities)
			{
				Transform* transform = entityManager.getComponent<Transform>(entity);
				MeshRender* meshData = entityManager.getComponent<MeshRender>(entity);

				RenderObject renderObj;
				renderObj.material = meshData->material;
				renderObj.mesh = meshData->mesh;
				renderObj.transformMatrix = transform->calculateTransformationMatrix();
				renderObj.normalMatrix = transform->calculateNormalMatrix();
				renderObjects.push_back(renderObj);
			}
		}

		std::vector<RenderObject> renderObjects{};
	};
}