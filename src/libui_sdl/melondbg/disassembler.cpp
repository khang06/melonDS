#include "disassembler.h"
#include "../imgui/imgui.h"
#include <capstone/capstone.h>
#include "../../NDS.h"

namespace melonDBG {
DisassemblerWindow::DisassemblerWindow() {
    if (cs_open(CS_ARCH_ARM, CS_MODE_ARM, &arm_handle) != CS_ERR_OK)
        exit(-1);
    memset(goto_buffer, 0, sizeof(goto_buffer));
    memset(cached_goto_buffer, 0, sizeof(cached_goto_buffer));
    memset(temp_title, 0, sizeof(temp_title));
}

// TODO: is it worth caching disassembled instructions?
// also, thumb support
void DisassemblerWindow::Draw() {
    cs_insn *insn;
    size_t count;

    if (ImGui::Begin(GetTitle().c_str(), &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar)) {

        // draw menu
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                ImGui::InputText("Name", temp_title, sizeof(temp_title));
                if (ImGui::Button("Apply")) { SetTitle(std::string(temp_title)); }
                ImGui::EndMenu();
            } 
            ImGui::EndMenuBar();
        }

        // using spaces to align works well enough for me, column api sucks
        ImGui::Text("Address "); ImGui::SameLine();
        ImGui::Text("Hex     "); ImGui::SameLine();
        ImGui::Text("Instruction");
        ImGui::Separator();

        ImGui::BeginChild("##scrolling", ImVec2(0, -24), false, ImGuiWindowFlags_NoMove);
        const uint32_t base_addr = 0x2000000; // main ram offset
        uint8_t data[4]; // stores current instruction
        char address_buffer[9];
        ImGuiListClipper clipper(sizeof(NDS::MainRAM) / 4, ImGui::GetTextLineHeight());
        for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; line++) {
            const uint32_t addr = line * 4;
            memcpy(data, &NDS::MainRAM[addr], 4);

            snprintf(address_buffer, sizeof(address_buffer), "%08X", base_addr + addr);
            if (ImGui::Selectable(address_buffer, line == selected)) { // address
                selected = line;
                snprintf(goto_buffer, sizeof(goto_buffer), "%08X", base_addr + addr);
                memcpy(cached_goto_buffer, goto_buffer, 8);
            }
            ImGui::SameLine();
            ImGui::Text("%02X%02X%02X%02X", data[0], data[1], data[2], data[3]); // instruction as hex
            ImGui::SameLine();
            count = cs_disasm(arm_handle, (const uint8_t*)data, 4, base_addr + addr, 0, &insn); // decoded instruction
            if (count != 0)
                ImGui::Text("%s %s", insn[0].mnemonic, insn[0].op_str);
            else
                ImGui::Text(" ");

            cs_free(insn, count);
        }

        clipper.End();
        CalcNotImplementedModal();
        ImGui::EndChild();

        ImGui::InputText(" ", goto_buffer, 9, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);

        // calculate goto input
        if (memcmp(goto_buffer, cached_goto_buffer, sizeof(goto_buffer))) {
            ImGui::BeginChild("##scrolling");
            uint32_t goto_addr = 0;
            try {
                goto_addr = std::stoul(goto_buffer, nullptr, 16);
            } catch(...) {} // this is awful but i don't know how else i can handle this
            if (goto_addr > base_addr && goto_addr < base_addr + sizeof(NDS::MainRAM)) {
                ImGui::SetScrollY(((goto_addr - base_addr) / 4) * ImGui::GetTextLineHeight());
                memcpy(cached_goto_buffer, goto_buffer, 8);
                cached_goto_buffer[8] = '\0';
            }
            ImGui::EndChild();
        }

        ImGui::End();
    }
}
}