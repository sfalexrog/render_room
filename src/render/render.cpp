#include "render.h"

#include <vector>
#include <glbinding/gl43core/gl.h>

#include "format/mesh.h"
#include "util/Logger.h"
#include "util/Config.h"

namespace cmcray
{

    //namespace
    //{
        struct RenderMesh
        {
            gl::GLuint vbo;
            gl::GLuint ibo;
            size_t idxSize;
            glm::mat4 transform;
            RenderMesh(gl::GLuint vbo, gl::GLuint ibo, size_t idxSize, glm::mat4 transform) : vbo(vbo), ibo(ibo), idxSize(idxSize), transform(transform) {}
        };

        struct RenderState
        {
            gl::GLuint vao;
            std::vector<RenderMesh> sceneObjects;
        } state;





    //}

    namespace Renderer
    {

        void init()
        {
            gl::glCreateVertexArrays(1, &state.vao);
        }

        void addToScene(const Mesh& object, const glm::mat4 transform)
        {
            gl::glBindVertexArray(state.vao);
            gl::GLuint meshVbo;
            gl::GLuint meshIbo;
            gl::glCreateBuffers(1, &meshVbo);
            gl::glCreateBuffers(1, &meshIbo);
            gl::glBindBuffer(gl::GL_ARRAY_BUFFER, meshVbo);
            gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(Mesh::Vertex) * object.v.size(), object.v.data(), gl::GL_STATIC_DRAW);
            gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, meshIbo);
            gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(object.indices[0]) * object.indices.size(), object.indices.data(), gl::GL_STATIC_DRAW);
            state.sceneObjects.emplace_back(meshVbo, meshIbo, object.indices.size(), transform);
        }

        void clearScene()
        {
            gl::glBindVertexArray(state.vao);
            for(const auto& obj : state.sceneObjects)
            {
                gl::glDeleteBuffers(1, &obj.vbo);
                gl::glDeleteBuffers(1, &obj.ibo);
            }
            state.sceneObjects.clear();
        }

        void render(Shader& shader, const Camera& camera)
        {
            // Re-enable for debugging purposes
#if 0
#define CHECK {gl::GLenum error; if ((error = gl::glGetError()) != gl::GL_NO_ERROR) Log(ERROR) << __FILE__ << ":" << __LINE__ << ": GL error: " << error;}
#else
#define CHECK
#endif
            auto view_projection = VPFromCamera(camera);
            auto view_mtx = view_projection.first;
            auto projection_mtx = view_projection.second;
            gl::glBindVertexArray(state.vao); CHECK;
            shader.activate(); CHECK;

            gl::glEnable(gl::GL_DEPTH_TEST); CHECK;
            gl::glEnable(gl::GL_BLEND);
            gl::glPolygonMode(gl::GL_FRONT_AND_BACK, gl::GL_FILL);

            for(const auto& obj : state.sceneObjects)
            {
                auto mvp = projection_mtx * view_mtx * obj.transform;
                float lightIntensity = Config::lightIntensity * Config::scaleFactor * Config::scaleFactor;
                shader.bind(0, obj.transform); CHECK;
                      shader.bind(1, view_mtx); CHECK;
                      shader.bind(2, mvp); CHECK;
                      shader.bind(3, glm::vec3{1.0f, 1.0f, 1.0f}); CHECK;
                      shader.bind(4, lightIntensity); CHECK;
                      shader.bind(5, view_mtx * obj.transform); CHECK;
                shader.bind(6, camera.position); CHECK;

                gl::glBindBuffer(gl::GL_ARRAY_BUFFER, obj.vbo); CHECK;
                gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, obj.ibo); CHECK;

                gl::glEnableVertexAttribArray(0); CHECK;
                gl::glVertexAttribPointer(0, 3, gl::GL_FLOAT, gl::GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, pos)); CHECK;
                gl::glEnableVertexAttribArray(1); CHECK;
                gl::glVertexAttribPointer(1, 3, gl::GL_FLOAT, gl::GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, norm)); CHECK;

                gl::glDrawElements(gl::GL_TRIANGLES, obj.idxSize, gl::GL_UNSIGNED_INT, 0); CHECK;
            }
        }
    }

}