#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_memory_editor.h"
#include <vector>
#include "window.h"

namespace melonDBG {
class MemoryEditorWindow : public Window {
public:
    MemoryEditorWindow();
    void Draw();
private:
    MemoryEditor mem_edit;
};
}