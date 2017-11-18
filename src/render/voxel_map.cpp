#include <glbinding/gl43core/gl.h>
#include "voxel_map.h"

namespace cmcray
{

    VoxelMap::VoxelMap(int x, int y, int z) : _sizeX(x), _sizeY(y), _sizeZ(z)
    {
        gl::glGenTextures(1, &_texHandle);
        gl::glTextureStorage3D(_texHandle, 1, gl::GL_R32F, _sizeX, _sizeY, _sizeZ);
    }

    VoxelMap::~VoxelMap()
    {
        gl::glDeleteTextures(1, &_texHandle);
    }

    void VoxelMap::getData(void *data, size_t dataSz)
    {
        if (dataSz < (_sizeX * _sizeY * _sizeZ * 4))
        {
            return;
        }
    }


}