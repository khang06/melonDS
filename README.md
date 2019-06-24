# ~~melonDS~~ melonDBG

hi, this is a fork of melonDS that adds a debugger. it's **very** unfinished at the moment

### TODO (in no particular order)

- [x] get my own window with imgui
- [x] window system (barebones for now)
- [ ] clean up the cmakelists
    - [ ] get things like `#include <stuff/blah.h>` working again so i don't have to do relative paths
    - [ ] split things into projects properly
- [ ] write a better readme
- [x] arm register windows
    - [ ] copy register to clipboard
    - [ ] edit registers
    - [ ] go to address in memory viewer/disassembler
- [x] memory viewer window (basically just the one from https://github.com/ocornut/imgui_club, but customized a bit)
    - [ ] view memory regions outside of main ram
    - [ ] make data changes easier to see, like cheat engine
- [x] disassembler window
    - [ ] disassemble memory regions outside of main ram
    - [ ] right-click context menu
        - [ ] assembler
        - [ ] copy values (address, decoded instruction, etc)
    - [ ] breakpoints **(!!!)**
    - [ ] syntax highlighting
- [ ] io register window
- [ ] vram window
- [ ] rom info window
