#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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
                    output.vertices.emplace_back(x, y, z);
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
            return output;
        }
    }
}
