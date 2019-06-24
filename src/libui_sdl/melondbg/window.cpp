#include <string>
#include "window.h"
#include "../imgui/imgui.h"

namespace melonDBG {
void Window::SetTitle(std::string name) {
    title = name;
}

const std::string& Window::GetTitle() {
    return title;
}

void NotImplementedModal() {
    ImGui::OpenPopup("Not implemented!!!");
}

void CalcNotImplementedModal() {
    if (ImGui::BeginPopupModal("Not implemented!!!", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This feature currently isn't implemented. Sorry.");
        if (ImGui::Button("OK", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
    }
}
}