#pragma once
#include "window.h"
#include <capstone/capstone.h>
#include "../../NDS.h"

namespace melonDBG {
class DisassemblerWindow : public Window {
public:
    DisassemblerWindow();
    void Draw();
private:
    // TODO: Thumb
    csh arm_handle;
    char goto_buffer[9];
    char cached_goto_buffer[9];
    uint32_t cached_goto = 0;
    int32_t selected = -1;
    char temp_title[0x100];
};
}