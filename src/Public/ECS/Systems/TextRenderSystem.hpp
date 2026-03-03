#pragma once
#include "ECS/System.hpp"
#include "ECS/BaseComponents.hpp"
#include "Rendering/UIRenderer.hpp"

namespace crsp {
	class TextRenderSystem : public BaseSystem<Transform2D, TextRender> {
	public:
		TextRenderSystem(EntityManager& entityManager, Window& window) : BaseSystem(entityManager), window(window) {}
		~TextRenderSystem() = default;

		void update(float deltaTime, float currentTime) override;

		std::vector<UIRenderObject> renderObjects{};
		Window& window;
	};
}