#include "SceneLogic/GameObject.hpp"
#include "SceneLogic/Scene.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <type_traits>

namespace crsp {
	void GameObject::postUpdate() {
		for (auto& component : components)
		{
			component->render();
		}
	}

    glm::mat4 Transform::calculateTransformationMatrix()
    {
#if 1
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{position.x, position.y, position.z, 1.0f} };
#else
		glm::mat4 model{ 1.0f };

		model = glm::translate(model, position);

		// yaw (Y axis)
		model = glm::rotate(model, rotation.y, glm::vec3{ 0.0f, -1.0f, 0.0f });

		// pitch (X axis)
		model = glm::rotate(model, rotation.x, glm::vec3{ 1.0f, 0.0f, 0.0f });

		model = glm::scale(model, scale);

		return model;
#endif
    }

    glm::mat4 Transform::calculateNormalMatrix()
    {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 invScale = 1.0f / scale;

		return glm::mat3{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				invScale.z * (c2 * s1),
				invScale.z * (-s2),
				invScale.z * (c1 * c2),
			} };
    }

	glm::mat3 Transform::calculateTransformationMatrix2D()
	{
		const float c = glm::cos(rotation.z);
		const float s = glm::sin(rotation.z);

		return glm::mat4{
			{
				scale.x * c,   scale.x * s,   0.0f,   0.0f,
			},
			{
			   -scale.y * s,   scale.y * c,   0.0f,   0.0f,
			},
			{
				0.0f,          0.0f,          1.0f,   0.0f,
			},
			{
				position.x,    position.y,    0.0f,   1.0f,
			}
		};
	}
} // namespace