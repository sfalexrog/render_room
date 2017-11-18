//
// Created by sf on 18/11/17.
//

#ifndef CMC_RAYMARCH_MESH_H
#define CMC_RAYMARCH_MESH_H

#include <glm/vec3.hpp>
#include <vector>

namespace cmcray
{
    struct Mesh
    {
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;

        Mesh() : vertices(), indices() {}
    };
}

#endif //CMC_RAYMARCH_MESH_H
