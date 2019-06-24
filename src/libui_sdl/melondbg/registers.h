#pragma once
#include "../../NDS.h"
#include "window.h"

namespace melonDBG {
class RegisterWindow : public Window {
public:
    RegisterWindow(const std::string& new_title, ARM* arm) : Window() {
        arm_obj = arm;
        SetTitle(new_title);
    };
    void Draw();
private:
    ARM* arm_obj = nullptr;
};
}