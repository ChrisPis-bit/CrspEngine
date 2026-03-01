#include "SceneLogic/UIRenderComponent.hpp"
#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/Scene.hpp"

namespace crsp {
	void UIRenderComponent::render()
	{
		if (isActive()) owner.getScene().renderUI(getRenderData());
	}

	UIRenderObject UIRenderComponent::getRenderData()
	{
		UIRenderObject renderObj;
		renderObj.material = material;
		renderObj.mesh = mesh;
		renderObj.transformMatrix = owner.transform.calculateTransformationMatrix();
		return renderObj;
	}
}