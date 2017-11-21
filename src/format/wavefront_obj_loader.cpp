#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

#include "wavefront_obj_loader.h"

namespace cmcray
{
    namespace Loader
    {
        Mesh loadObj(const std::string& fileName)
        {
            std::ifstream input(fileName);
            Mesh output;
            std::string line;
            while(input)
            {
                std::getline(input, line);
                std::istringstream parsedLine(line);
                std::string recordType;
                parsedLine >> recordType;
                if (recordType == "v")
                {
                    // Vertex data
                    float x, y, z, w;
                    w = 1;
                    parsedLine >> x >> y >> z;
                    if ((parsedLine >> w))
                    {
                        x /= w;
                        y /= w;
                        z /= w;
                    }
                    output.v.push_back(Mesh::Vertex{glm::vec3{x, y, z}, glm::vec3{}});
                }
                else if (recordType == "f")
                {
                    // Index data, indexing starts at 1
                    uint32_t v1, v2, v3;
                    parsedLine >> v1 >> v2 >> v3;
                    output.indices.emplace_back(v1 - 1);
                    output.indices.emplace_back(v2 - 1);
                    output.indices.emplace_back(v3 - 1);
                }
            }
            // Precalculate normals for triangles
            for(size_t i = 0; i < output.indices.size(); i += 3)
            {
                uint32_t i1, i2, i3;
                i1 = output.indices[i];
                i2 = output.indices[i + 1];
                i3 = output.indices[i + 2];
                auto v1 = output.v[i1].pos;
                auto v2 = output.v[i2].pos;
                auto v3 = output.v[i3].pos;
                auto e1 = v2 - v1;
                auto e2 = v3 - v1;
                auto n = glm::normalize(glm::cross(e1, e2));
                output.v[i1].norm = n;
                output.v[i2].norm = n;
                output.v[i3].norm = n;
            }
            return output;
        }
    }
}
