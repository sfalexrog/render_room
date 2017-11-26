#include <imgui.h>
#include <glm/glm.hpp>
#include <array>
#include "format/mesh.h"
#include "MapWindow.h"
#include "render/camera.h"

#include <glbinding/gl43core/gl.h>
#include "render/shader.h"
#include "render/voxel_map.h"

#include <fstream>
#include <sys/stat.h>

#include "util/Config.h"
#include "util/Logger.h"

#include "stb/stb_image.h"

namespace cmcray
{

    namespace
    {
        bool isVoxrenderInit = false;

        struct {
            gl::GLuint vao;
            gl::GLuint vbo;
            gl::GLuint ibo;
            gl::GLuint fb;
            gl::GLuint rb;

            gl::GLuint textureId;
            gl::GLuint depthBuffer;

            gl::GLuint colorMapId;

            Shader *shader;

        } renderState;

        void initVoxRender()
        {
            const glm::vec2 quad[] = {{-1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}};
            const gl::GLuint indices[] = {0, 1, 2, 3};

            gl::glGenVertexArrays(1, &renderState.vao);
            gl::glBindVertexArray(renderState.vao);
            gl::glGenBuffers(1, &renderState.vbo);
            gl::glBindBuffer(gl::GL_ARRAY_BUFFER, renderState.vbo);
            gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, quad, gl::GL_STATIC_DRAW);
            gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, renderState.ibo);
            gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(gl::GLuint) * 4, indices, gl::GL_STATIC_DRAW);
            gl::glGenFramebuffers(1, &renderState.fb);
            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, renderState.fb);

            gl::glGenTextures(1, &renderState.textureId);
            gl::glBindTexture(gl::GL_TEXTURE_2D, renderState.textureId);

            auto voxmapSize = Config::voxelMapDimensions;

            gl::glTexImage2D(gl::GL_TEXTURE_2D, 0, gl::GL_RGB, voxmapSize.x, voxmapSize.z, 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, 0);

            gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
            gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);

            gl::glFramebufferTexture2D(gl::GL_FRAMEBUFFER, gl::GL_COLOR_ATTACHMENT0, gl::GL_TEXTURE_2D, renderState.textureId, 0);

            gl::glGenRenderbuffers(1, &renderState.rb);
            gl::glBindRenderbuffer(gl::GL_RENDERBUFFER, renderState.rb);
            gl::glRenderbufferStorage(gl::GL_RENDERBUFFER, gl::GL_DEPTH24_STENCIL8, voxmapSize.x, voxmapSize.z);

            gl::glFramebufferRenderbuffer(gl::GL_FRAMEBUFFER, gl::GL_DEPTH_STENCIL_ATTACHMENT, gl::GL_RENDERBUFFER, renderState.rb);

            if (gl::glCheckFramebufferStatus(gl::GL_FRAMEBUFFER) != gl::GL_FRAMEBUFFER_COMPLETE)
            {
                Log(ERROR) << "Framebuffer incomplete, won't be able to render map properly";
            }

            renderState.shader = new Shader();

            {
                const char *vertShaderFile = "shaders/texrender.vert";
                struct ::stat sb;
                if (!::stat(vertShaderFile, &sb)) {
                    std::vector<char> vertShaderSrc(sb.st_size + 1, '\0');
                    std::ifstream in(vertShaderFile);
                    in.read(vertShaderSrc.data(), vertShaderSrc.size());
                    renderState.shader->source(vertShaderSrc.data(), Shader::Type::Vertex);
                }
                const char *fragShaderFile = "shaders/texrender.frag";
                if (!::stat(fragShaderFile, &sb)) {
                    std::vector<char> fragShaderSrc(sb.st_size + 1, '\0');
                    std::ifstream in(fragShaderFile);
                    in.read(fragShaderSrc.data(), fragShaderSrc.size());
                    renderState.shader->source(fragShaderSrc.data(), Shader::Type::Fragment);
                }

                renderState.shader->link();
            }

            gl::glGenTextures(1, &renderState.colorMapId);
            {
                int x, y, channels;
                auto image = stbi_load("colormap.png", &x, &y, &channels, 3);
                assert(image != 0);
                gl::glBindTexture(gl::GL_TEXTURE_1D, renderState.colorMapId);
                gl::glTexImage1D(gl::GL_TEXTURE_1D, 0, gl::GL_RGB8, x, 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, image);
                stbi_image_free(image);

                gl::glTexParameteri(gl::GL_TEXTURE_1D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
                gl::glTexParameteri(gl::GL_TEXTURE_1D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);
                gl::glTexParameteri(gl::GL_TEXTURE_1D, gl::GL_TEXTURE_WRAP_R, gl::GL_CLAMP_TO_EDGE);
            }

            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);
            isVoxrenderInit = true;
        }

        void renderVoxMap(VoxelMap &voxMap)
        {
            gl::glBindVertexArray(renderState.vao);

            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, renderState.fb);
            gl::GLint viewport[4];
            gl::glGetIntegerv(gl::GL_VIEWPORT, viewport);
            gl::glViewport(0, 0, Config::voxelMapDimensions.x, Config::voxelMapDimensions.z);

            gl::glBindBuffer(gl::GL_ARRAY_BUFFER, renderState.vbo);
            gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, renderState.ibo);

            gl::glActiveTexture(gl::GL_TEXTURE0);
            gl::glBindTexture(gl::GL_TEXTURE_3D, voxMap.hndl());
            gl::glActiveTexture(gl::GL_TEXTURE1);
            gl::glBindTexture(gl::GL_TEXTURE_1D, renderState.colorMapId);


            renderState.shader->activate();
            gl::glUniform1i(0, 0);
            gl::glUniform1i(1, 1);
            gl::glEnableVertexAttribArray(0);
            gl::glVertexAttribPointer(0, 2, gl::GL_FLOAT, gl::GL_FALSE, 0, 0);

            gl::glDrawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);

            gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);
            gl::glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
    }

    namespace MapWindow
    {

        std::array<glm::vec2, 4> getBounds(const Mesh& mesh)
        {
            float xmin, xmax, ymin, ymax;
            auto first_vertex = mesh.v[0].pos;
            xmin = xmax = first_vertex.x;
            ymin = ymax = first_vertex.z;
            for(const auto& v : mesh.v)
            {
                xmin = std::min(xmin, v.pos.x);
                xmax = std::max(xmax, v.pos.x);
                ymin = std::min(ymin, v.pos.z);
                ymax = std::max(ymax, v.pos.z);
            }
            return {glm::vec2{xmin, ymin}, glm::vec2{xmax, ymax}, glm::vec2{}, glm::vec2{}};
        };

        void draw(const Mesh& mesh, const Camera& camera, VoxelMap& voxMap)
        {
            if (!isVoxrenderInit)
            {
                initVoxRender();
            }

            renderVoxMap(voxMap);

            auto bounds = getBounds(mesh);

            float size_x = 800.0f;
            float size_y = 620.0f;


            ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_Once);
            ImGui::Begin("Room Mesh");

            auto win_min = ImGui::GetWindowContentRegionMin();
            auto win_max = ImGui::GetWindowContentRegionMax();
            size_x = win_max.x - win_min.x;
            size_y = win_max.y - win_min.y;

            const float mx = (size_x - 15) / (bounds[1].x - bounds[0].x);
            const float my = (size_y - 15) / (bounds[1].y - bounds[0].y);

            const float m = std::min(mx, my);

            const float x0 = -m * bounds[0].x;
            const float y0 = -m * bounds[0].y;

            ImGui::Text("X: %7.3f, Y: %7.3f, Z: %7.3f", camera.position.x, camera.position.y, camera.position.z);

            auto drawList = ImGui::GetWindowDrawList();
            auto p = ImGui::GetCursorScreenPos();

            auto imageTopLeft = ImVec2(p.x + 1, p.y + 1);
            auto imageBottomRight = ImVec2(p.x + (bounds[1] - bounds[0]).x * m + 1, p.y + (bounds[1] - bounds[0]).y * m + 1);

            drawList->AddImage((ImTextureID)renderState.textureId, imageTopLeft, imageBottomRight);

            for (size_t i = 0; i < mesh.indices.size(); i += 3)
            {
                ImVec2 a, b, c;
                auto v1 = mesh.v[mesh.indices[i]].pos;
                auto v2 = mesh.v[mesh.indices[i + 1]].pos;
                auto v3 = mesh.v[mesh.indices[i + 2]].pos;
                a = ImVec2(p.x + x0 + v1.x * m, p.y + y0 + v1.z * m);
                b = ImVec2(p.x + x0 + v2.x * m, p.y + y0 + v2.z * m);
                c = ImVec2(p.x + x0 + v3.x * m, p.y + y0 + v3.z * m);
                drawList->AddTriangle(a, b, c, ImColor(0, 255, 0));
            }

            // Draw camera
            auto camPos = camera.position;
            auto leftBeam = glm::vec3{cos(camera.fov/2 + camera.rotation.y), 0.0f, -sin(camera.fov/2 + camera.rotation.y)};
            auto rightBeam = glm::vec3{cos(-camera.fov/2 + camera.rotation.y), 0.0f, -sin(-camera.fov/2 + camera.rotation.y)};
            auto camLeft = camPos + leftBeam * camera.far;
            auto camRight = camPos + rightBeam * camera.far;
            auto a = ImVec2(p.x + x0 + camPos.x * m, p.y + y0 + camPos.z * m);
            auto b = ImVec2(p.x + x0 + camLeft.x * m, p.y + y0 + camLeft.z * m);
            auto c = ImVec2(p.x + x0 + camRight.x * m, p.y + y0 + camRight.z * m);

            drawList->AddTriangle(a, b, c, ImColor(255, 255, 0));

            ImGui::End();
        }
    }
}
