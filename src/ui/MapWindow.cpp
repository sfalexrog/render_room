#include <imgui.h>
#include <glm/glm.hpp>
#include <array>
#include "format/mesh.h"
#include "MapWindow.h"
#include "render/camera.h"

namespace cmcray
{
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

        void draw(const Mesh& mesh, const Camera& camera)
        {

            auto bounds = getBounds(mesh);

            const float size_x = 800.0f;
            const float size_y = 620.0f;

            const float mx = (size_x - 40) / (bounds[1].x - bounds[0].x);
            const float my = (size_y - 40) / (bounds[1].y - bounds[0].y);

            const float m = std::min(mx, my);

            const float x0 = -m * bounds[0].x;
            const float y0 = -m * bounds[0].y;

            ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_Always);
            ImGui::Begin("Room Mesh", nullptr, ImGuiWindowFlags_NoResize);

            ImGui::Text("X: %7.3f, Y: %7.3f, Z: %7.3f", camera.position.x, camera.position.y, camera.position.z);

            auto drawList = ImGui::GetWindowDrawList();
            auto p = ImGui::GetCursorScreenPos();

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
