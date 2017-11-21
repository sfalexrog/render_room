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
        struct Vertex {
            glm::vec3 pos;
            glm::vec3 norm;
        };
        std::vector<Vertex> v;
        //std::vector<glm::vec3> vertices;
        //std::vector<glm::vec3> normals;
        std::vector<uint32_t> indices;

        Mesh() : v{}, indices{} {}
    };
}

#endif //CMC_RAYMARCH_MESH_H
