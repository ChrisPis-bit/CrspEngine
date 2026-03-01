#include "SceneLogic/MeshRenderComponent.hpp"
#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/Scene.hpp"

namespace crsp {
	void MeshRenderComponent::render()
	{
		if (isActive()) owner.getScene().renderSurface(getRenderData());
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