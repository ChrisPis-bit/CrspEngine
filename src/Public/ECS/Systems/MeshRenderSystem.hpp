#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "Rendering/MaterialRenderer.hpp"

namespace crsp {
	class MeshRenderSystem : public BaseSystem<Transform, MeshRender>{
	public:
		MeshRenderSystem(EntityManager& entityManager) : BaseSystem(entityManager) {}
		~MeshRenderSystem() = default;

		void update(float deltaTime, float currentTime) override;

		std::vector<RenderObject> renderObjects{};
	};
}