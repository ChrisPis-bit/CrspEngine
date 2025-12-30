#include "Rendering/DirectionalLight.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace crsp {
    void DirectionalLight::setOrthographicProjection(
        float left, float right, float top, float bottom, float near, float far) {
        projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
    }

    void DirectionalLight::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
        const glm::vec3 w{ glm::normalize(direction) };
        const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
        const glm::vec3 v{ glm::cross(w, u) };

        viewMatrix = glm::mat4{ 1.f };
        viewMatrix[0][0] = u.x;
        viewMatrix[1][0] = u.y;
        viewMatrix[2][0] = u.z;
        viewMatrix[0][1] = v.x;
        viewMatrix[1][1] = v.y;
        viewMatrix[2][1] = v.z;
        viewMatrix[0][2] = w.x;
        viewMatrix[1][2] = w.y;
        viewMatrix[2][2] = w.z;
        viewMatrix[3][0] = -glm::dot(u, position);
        viewMatrix[3][1] = -glm::dot(v, position);
        viewMatrix[3][2] = -glm::dot(w, position);
    }

    glm::mat4 DirectionalLight::getProjectionViewMatrix()
    {
        return projectionMatrix * viewMatrix;
    }
} // namespace