//
// Created by sf on 16/11/17.
//

#ifndef CMC_RAYMARCH_VOXEL_MAP_H
#define CMC_RAYMARCH_VOXEL_MAP_H

#include <glbinding/gl43core/gl.h>
#include <glm/vec3.hpp>

namespace cmcray
{
    class VoxelMap
    {
    private:
        int _sizeX, _sizeY, _sizeZ;
        gl::GLuint _texHandle;

    public:
        VoxelMap(glm::ivec3 dims) : VoxelMap(dims.x, dims.y, dims.z) {}
        VoxelMap(int x, int y, int z);
        ~VoxelMap();

        int sx() { return _sizeX; }
        int sy() { return _sizeY; }
        int sz() { return _sizeZ; }

        gl::GLuint hndl() { return _texHandle; }

        void getData(void* data, size_t dataSz);

        // Will dump all images (SLOWLY)
        void dumpImages(const char* prefix);

    };
}

#endif //CMC_RAYMARCH_VOXEL_MAP_H
