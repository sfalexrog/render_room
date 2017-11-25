#ifndef CMC_RAYMARCH_COMPUTE_H
#define CMC_RAYMARCH_COMPUTE_H

#include "format/mesh.h"
#include "render/shader.h"

#include "render/voxel_map.h"

namespace cmcray
{
    namespace Compute
    {

        void init();
        void addToCompute(const Mesh& mesh);
        void compute(Shader& shader, VoxelMap& result);
        bool finished();
    }
}

#endif //CMC_RAYMARCH_COMPUTE_H
