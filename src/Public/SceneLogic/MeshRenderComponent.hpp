#pragma once

#include "SceneLogic/Component.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/MaterialRenderer.hpp"
#include <memory>

namespace crsp {
	/// <summary>
	/// Component for rendering 3D meshes.
	/// </summary>
	class MeshRenderComponent : public Component {
	public:
		MeshRenderComponent(GameObject& owner) : Component(owner) {}
		~MeshRenderComponent() = default;

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		void render() override;

	private:
		RenderObject getRenderData();

	};
}