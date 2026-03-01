#pragma once
#include "SceneLogic/UIRenderComponent.hpp"

namespace crsp {
	class TextRenderComponent : public UIRenderComponent {
	public:
		TextRenderComponent(GameObject& owner);
		std::string text;
	};
}