#include <imgui.h>
#include "util/InteractiveLog.h"
#include "LogWindow.h"

#include <string>

namespace cmcray
{
    namespace LogWindow
    {

        namespace
        {
            const char *WindowName = "Application Log";
            bool scrollToLast = true;
        }

        void draw()
        {
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
            ImGui::Begin(WindowName);
            if (ImGui::Button("Clear")) InteractiveLog::clearLog();
            ImGui::SameLine();
            ImGui::Checkbox("Scroll to last", &scrollToLast);
            ImGui::Separator();
            ImGui::BeginChild("scrolling");
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
            for(size_t i = 0; i < InteractiveLog::getLogSize(); ++i)
            {
                ImGui::TextUnformatted(InteractiveLog::getLogLineAt(i).c_str());
            }
            if (scrollToLast)
            {
                ImGui::SetScrollHere(1.0f);
            }

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::End();
        }
    }
}
