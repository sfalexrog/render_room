#include <glbinding/gl43core/gl.h>
#include "voxel_map.h"
#include <vector>
#include <string>
#include <sstream>

// FIXME: Maybe not a good file to include the IMAGE_IMPLEMENTATION in?
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

namespace cmcray
{

    VoxelMap::VoxelMap(int x, int y, int z) : _sizeX(x), _sizeY(y), _sizeZ(z)
    {
        gl::glGenTextures(1, &_texHandle);
        gl::glBindTexture(gl::GL_TEXTURE_3D, _texHandle);
        gl::glTexStorage3D(gl::GL_TEXTURE_3D, 1, gl::GL_R32F, _sizeX, _sizeY, _sizeZ);
        std::vector<float> nullTexData(_sizeX * _sizeY * _sizeZ, 0.0f);
        gl::glTexSubImage3D(gl::GL_TEXTURE_3D, 0, 0, 0, 0, _sizeX, _sizeY, _sizeZ, gl::GL_RED, gl::GL_FLOAT, nullTexData.data());
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
        gl::glBindTexture(gl::GL_TEXTURE_3D, _texHandle);
        gl::glGetTexImage(gl::GL_TEXTURE_3D, 0, gl::GL_RED, gl::GL_FLOAT, data);
    }

    void VoxelMap::dumpImages(const char* prefix)
    {
        std::vector<float> texData(_sizeX * _sizeY * _sizeZ);
        getData(texData.data(), texData.size() * sizeof(float));
        for (int i = 0; i < _sizeZ; ++i)
        {
            std::stringstream ss;
            ss << prefix << "_" << i << ".png";
            std::vector<uint8_t> sliceData(_sizeX * _sizeY * 3);
            for(int x = 0; x < _sizeX; ++x)
            {
                for(int y = 0; y < _sizeY; ++y)
                {
                    sliceData[3 * (x + _sizeX * y)    ] = uint8_t(255.0f * (texData[x + _sizeX * i + _sizeX * _sizeY * y] / 100.0f));
                    sliceData[3 * (x + _sizeX * y) + 1] = uint8_t(255.0f * (texData[x + _sizeX * i + _sizeX * _sizeY * y] / 100.0f));
                    sliceData[3 * (x + _sizeX * y) + 2] = uint8_t(255.0f * (texData[x + _sizeX * i + _sizeX * _sizeY * y] / 100.0f));
                }
            }
            stbi_write_png(ss.str().c_str(), _sizeX, _sizeY, 3, sliceData.data(), _sizeX * 3);
        }
    }
}