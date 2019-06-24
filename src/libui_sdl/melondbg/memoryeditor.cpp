#include "memoryeditor.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_memory_editor.h"
#include "../../NDS.h"
#include <vector>

#define MAIN_RAM_OFFSET 0x2000000

namespace melonDBG {
MemoryEditorWindow::MemoryEditorWindow() {
    strncpy(mem_edit.Title, "Memory Editor", sizeof(mem_edit.Title));
}

void MemoryEditorWindow::Draw() {
    // evil const_cast!!!
    mem_edit.DrawWindow(const_cast<const char*>(mem_edit.Title), NDS::MainRAM, sizeof(NDS::MainRAM), MAIN_RAM_OFFSET);
    // hacky, but still works
    // mem_edit.Title only exists so it can be changed through the window
    SetTitle(mem_edit.Title);
    open = mem_edit.Open;
}
}