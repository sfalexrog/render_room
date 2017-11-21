#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace cmcray
{
    namespace
    {
        Camera defaultCamera;
    }

    Camera& getDefaultCamera()
    {
        return defaultCamera;
    }

    std::pair<glm::mat4, glm::mat4> VPFromCamera(const Camera& camera)
    {
        glm::vec3 direction {1.0f, 0.0f, 0.0f};
        direction = glm::rotate(direction, camera.rotation.y, glm::vec3{0.0f, 1.0f, 0.0f});
        glm::mat4 view = glm::lookAt(camera.position, camera.position + direction, glm::vec3{0.0f, 1.0f, 0.0f});
        glm::mat4 projection = glm::perspectiveFov(camera.fov, camera.targetSize.x, camera.targetSize.y, camera.near, camera.far);

        return std::make_pair(view, projection);
    };
}
