#pragma once

#include "glm/glm.hpp"

#include "Rendering/Mesh.hpp"

#include <memory>

namespace crsp {
	struct Transform {
		glm::vec2 pos;
	};

	class GameObject {
	public:

		std::shared_ptr<Mesh> mesh;
		Transform transform;
	};
} // namespace
