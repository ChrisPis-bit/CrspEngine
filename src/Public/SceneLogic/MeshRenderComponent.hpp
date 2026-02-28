#pragma once

#include "SceneLogic/Component.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/MaterialRenderer.hpp"

#include "SceneLogic/ComponentSystem.hpp"

#include <memory>

namespace crsp {
	/// <summary>
	/// Component for rendering 3D meshes.
	/// </summary>
	class MeshRenderComponent : public Component {
	public:
		MeshRenderComponent(GameObject& owner) : Component(owner) {}

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		RenderObject getRenderData();

	};

	/// <summary>
	/// Handles returning all render data from render components.
	/// </summary>
	class MeshRenderComponentSystem : public ComponentSystem<MeshRenderComponent> {
	public:
		MeshRenderComponentSystem() = default;

		void update() override;
		void lateUpdate() override; 

		std::vector<RenderObject> copyRenderQueue() { return renderQueue; }

	private:
		std::vector<RenderObject> renderQueue;
	};
}