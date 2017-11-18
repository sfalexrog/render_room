#include <imgui.h>
#include <glm/glm.hpp>
#include <array>
#include "format/mesh.h"
#include "MapWindow.h"

namespace cmcray
{
    namespace MapWindow
    {

        std::array<glm::vec2, 4> getBounds(const Mesh& mesh)
        {
            float xmin, xmax, ymin, ymax;
            auto first_vertex = mesh.vertices[0];
            xmin = xmax = first_vertex.x;
            ymin = ymax = first_vertex.z;
            for(const auto& v : mesh.vertices)
            {
                xmin = std::min(xmin, v.x);
                xmax = std::max(xmax, v.x);
                ymin = std::min(ymin, v.z);
                ymax = std::max(ymax, v.z);
            }
            return {glm::vec2{xmin, ymin}, glm::vec2{xmax, ymax}, glm::vec2{}, glm::vec2{}};
        };

        void draw(const Mesh& mesh)
        {

            auto bounds = getBounds(mesh);

            const float size_x = 800.0f;
            const float size_y = 600.0f;

            const float mx = (size_x - 40) / (bounds[1].x - bounds[0].x);
            const float my = (size_y - 40) / (bounds[1].y - bounds[0].y);

            const float m = std::min(mx, my);

            const float x0 = -m * bounds[0].x;
            const float y0 =  m * bounds[1].y;


            ImGui::SetNextWindowSize(ImVec2(size_x, size_y), ImGuiCond_Always);
            ImGui::Begin("Room Mesh", nullptr, ImGuiWindowFlags_NoResize);

            auto drawList = ImGui::GetWindowDrawList();
            auto p = ImGui::GetCursorScreenPos();

            for (size_t i = 0; i < mesh.indices.size(); i += 3)
            {
                ImVec2 a, b, c;
                auto v1 = mesh.vertices[mesh.indices[i]];
                auto v2 = mesh.vertices[mesh.indices[i + 1]];
                auto v3 = mesh.vertices[mesh.indices[i + 2]];
                a = ImVec2(p.x + x0 + v1.x * m, p.y + y0 - v1.z * m);
                b = ImVec2(p.x + x0 + v2.x * m, p.y + y0 - v2.z * m);
                c = ImVec2(p.x + x0 + v3.x * m, p.y + y0 - v3.z * m);
                drawList->AddTriangle(a, b, c, ImColor(0, 255, 0));
            }


            ImGui::End();
        }
    }
}
