//
// Created by sf on 20/11/17.
//

#ifndef CMC_RAYMARCH_RENDER_H
#define CMC_RAYMARCH_RENDER_H

#include <glm/mat4x4.hpp>
#include "format/mesh.h"
#include "render/camera.h"
#include "render/shader.h"

namespace cmcray
{

    namespace Renderer
    {
        void init();

        void addToScene(const Mesh& object, const glm::mat4 transform = glm::mat4{1.0f});
        void clearScene();

        void render(Shader& shader, const Camera& camera);

    }

}

#endif //CMC_RAYMARCH_RENDER_H
