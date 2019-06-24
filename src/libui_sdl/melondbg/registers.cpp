#include "../imgui/imgui.h"
#include "../../NDS.h"
#include "registers.h"
#include <string>

namespace melonDBG {
void RegisterWindow::Draw() {
    ImGui::Begin(GetTitle().c_str(), &open, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("R0  %08X", arm_obj->R[0]);
    ImGui::Text("R1  %08X", arm_obj->R[1]);
    ImGui::Text("R2  %08X", arm_obj->R[2]);
    ImGui::Text("R3  %08X", arm_obj->R[3]);
    ImGui::Text("R4  %08X", arm_obj->R[4]);
    ImGui::Text("R5  %08X", arm_obj->R[5]);
    ImGui::Text("R6  %08X", arm_obj->R[6]);
    ImGui::Text("R7  %08X", arm_obj->R[7]);
    ImGui::Text("R8  %08X", arm_obj->R[8]);
    ImGui::Text("R9  %08X", arm_obj->R[9]);
    ImGui::Text("R10 %08X", arm_obj->R[10]);
    ImGui::Text("R11 %08X", arm_obj->R[11]);
    ImGui::Text("R12 %08X", arm_obj->R[12]);
    ImGui::Text("SP  %08X", arm_obj->R[13]);
    ImGui::Text("LR  %08X", arm_obj->R[14]);
    ImGui::Text("PC  %08X", arm_obj->R[15]);
    ImGui::End();
}
}