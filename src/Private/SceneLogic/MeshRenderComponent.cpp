#include "SceneLogic/MeshRenderComponent.hpp"
#include "SceneLogic/GameObject.hpp"

namespace crsp {
	void MeshRenderComponentSystem::update()
	{
	}
	void MeshRenderComponentSystem::lateUpdate()
	{
		// Collect all render data from this frame.
		renderQueue.clear();
		for (auto renderer : components) {
			if(renderer->isActive()) renderQueue.push_back(renderer->getRenderData());
		}
	}
	RenderObject MeshRenderComponent::getRenderData()
	{
		RenderObject renderObj;
		renderObj.material = material;
		renderObj.mesh = mesh;
		renderObj.transformMatrix = owner.transform.calculateTransformationMatrix();
		renderObj.normalMatrix = owner.transform.calculateNormalMatrix();
		return renderObj;
	}
}