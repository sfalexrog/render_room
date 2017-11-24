#include "compute.h"
#include <glbinding/gl43core/gl.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>

#include "render/voxel_map.h"
#include "util/Config.h"
#include "util/Logger.h"

using namespace glm;

namespace cmcray
{

    namespace
    {
        // BE SURE THIS IS SYNCED WITH YOUR COMPUTE SHADER
        struct Triangle
        {
            vec4 a;
            vec4 b;
            vec4 c;
        };

        std::vector<Triangle> triangles;

        struct
        {
            gl::GLuint vao;
            gl::GLuint ssbo;
            vec3 minBound;
            vec3 maxBound;
        } CompState;

    }

    namespace Compute
    {

        void init()
        {
            CompState.minBound = glm::vec3{100000.0f, 100000.0f, 100000.0f};
            CompState.maxBound = glm::vec3{-100000.0f, -100000.0f, -100000.0f};
            gl::glGenVertexArrays(1, &CompState.vao);
            gl::glBindVertexArray(CompState.vao);
            gl::glGenBuffers(1, &CompState.ssbo);
            gl::glBindBuffer(gl::GL_SHADER_STORAGE_BUFFER, CompState.ssbo);
        }

        void addToCompute(const Mesh& mesh)
        {
            for(int i = 0; i < mesh.indices.size(); i += 3)
            {
                Triangle t;
                t.a = vec4(mesh.v[mesh.indices[i]].pos, 1.0f);
                t.b = vec4(mesh.v[mesh.indices[i + 1]].pos, 1.0f);
                t.c = vec4(mesh.v[mesh.indices[i + 2]].pos, 1.0f);
                CompState.minBound.x = std::min({t.a.x, t.b.x, t.c.x, CompState.minBound.x});
                CompState.minBound.y = std::min({t.a.y, t.b.y, t.c.y, CompState.minBound.y});
                CompState.minBound.z = std::min({t.a.z, t.b.z, t.c.z, CompState.minBound.z});
                CompState.maxBound.x = std::max({t.a.x, t.b.x, t.c.x, CompState.maxBound.x});
                CompState.maxBound.y = std::max({t.a.y, t.b.y, t.c.y, CompState.maxBound.y});
                CompState.maxBound.z = std::max({t.a.z, t.b.z, t.c.z, CompState.maxBound.z});
                triangles.push_back(t);
            }
            gl::glBindVertexArray(CompState.vao);
            gl::glBindBuffer(gl::GL_SHADER_STORAGE_BUFFER, CompState.ssbo);
            gl::glBufferData(gl::GL_SHADER_STORAGE_BUFFER, sizeof(Triangle) * triangles.size(), triangles.data(), gl::GL_STATIC_DRAW);
            gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 1, CompState.ssbo);
            gl::glBindBuffer(gl::GL_SHADER_STORAGE_BUFFER, 0);
        }

        void compute(Shader& shader, VoxelMap& result)
        {

#if 1
#define CHECK {gl::GLenum error; if ((error = gl::glGetError()) != gl::GL_NO_ERROR) Log(ERROR) << __FILE__ << ":" << __LINE__ << ": GL error: " << error;}
#else
#define CHECK
#endif
            gl::glBindVertexArray(CompState.vao);
            shader.activate(); CHECK;
            gl::glBindImageTexture(0, result.hndl(), 0, gl::GL_TRUE, 0, gl::GL_READ_WRITE, gl::GL_R32I); CHECK;
            gl::glUniform1i(0, 0); CHECK;
            gl::glUniform3fv(1, 1, value_ptr(CompState.minBound)); CHECK;
            gl::glUniform3fv(2, 1, value_ptr(CompState.maxBound)); CHECK;
            gl::glUniform1f(3, Config::scaleFactor); CHECK;

            gl::glUniform3fv(4, 1, glm::value_ptr(Config::cameraPos)); CHECK;
            for(int i = 0; i < 4; ++i)
            {
                gl::glUniform1f(5, pi<float>() / (32 * 4));
                gl::glDispatchCompute(32, 32, 1); CHECK;
            }
            //gl::glMemoryBarrier(gl::GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); CHECK;
            gl::glFlush();
            gl::glFinish();
        }

    }

}
