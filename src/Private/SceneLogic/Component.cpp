#include "SceneLogic/Component.hpp"
#include "SceneLogic/GameObject.hpp"

namespace crsp {
	bool Component::isActive()
	{
		return owner.isActive;
	}
}
