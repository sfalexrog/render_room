//
// Created by sf on 20/11/17.
//

#ifndef CMC_RAYMARCH_CAMERA_H
#define CMC_RAYMARCH_CAMERA_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <utility>
#include <glm/gtc/constants.hpp>

namespace cmcray
{

    struct Camera
    {
        float fov;
        float near;
        float far;
        glm::vec2 targetSize;
        glm::vec3 position;
        glm::vec2 rotation;

        Camera() : fov(glm::pi<float>() / 2), near(0.1f), far(10000.0f),
                   targetSize(glm::vec2{100.0f, 100.0f}), position(glm::vec3{0.0f, 0.0f, 0.0f}),
                   rotation(glm::vec2{0.0f, 0.0f}) {}
    };

    Camera& getDefaultCamera();

    std::pair<glm::mat4, glm::mat4> VPFromCamera(const Camera& camera);

}

#endif //CMC_RAYMARCH_CAMERA_H
