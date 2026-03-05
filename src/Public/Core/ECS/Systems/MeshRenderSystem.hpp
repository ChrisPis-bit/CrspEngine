#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "Rendering/SurfaceRenderer.hpp"

namespace crsp {
	class MeshRenderSystem : public BaseSystem<Transform, MeshRender>{
	public:
		MeshRenderSystem(EntityManager& entityManager) : BaseSystem(entityManager) {}
		~MeshRenderSystem() = default;

		void lateUpdate(float deltaTime, float currentTime) override;

		std::vector<RenderObject> renderObjects{};
	};
}