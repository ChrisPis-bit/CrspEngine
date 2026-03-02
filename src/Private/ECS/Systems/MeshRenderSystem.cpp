#include "ECS/Systems/MeshRenderSystem.hpp"

namespace crsp {
	void MeshRenderSystem::update(float deltaTime, float currentTime)
	{
		renderObjects.clear();
		for (auto const& entity : entities)
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
}