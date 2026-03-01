#include "Rendering/Mesh.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/UIRenderer.hpp"
#include "SceneLogic/Component.hpp"

#include <memory>

namespace crsp {
	/// <summary>
	/// Component for rendering UI.
	/// </summary>
	class UIRenderComponent : public Component {
	public:
		UIRenderComponent(GameObject& owner) : Component(owner) {}

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		void render() override;

		UIRenderObject getRenderData();
	};
}