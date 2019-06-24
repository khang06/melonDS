#pragma once
#include <string>

namespace melonDBG {
// this is to make window handling a bit more unified
class Window {
public:
    void SetTitle(std::string name);
    const std::string& GetTitle();
    virtual void Draw() = 0;
    bool open = true;
    // TODO: maybe a drawmenu function for unified menu options
private:
    // TODO: can i do unicode here?
    std::string title = "";
};

void NotImplementedModal();
void CalcNotImplementedModal();
}