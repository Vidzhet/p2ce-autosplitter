#pragma once
#include "dx11hook/dx11hook.h"

#define IMGUI_SETSTYLE() imgui_setstyle()
#define IMGUI_RENDER() imgui_render()

extern bool imgui_menu;
extern bool demo_paused;
extern bool apply_def_pos;

void IMGUI_SETSTYLE();
void IMGUI_RENDER();

extern ImFont* def_fontX14;
extern ImFont* def_fontX11;
extern ImFont* bold_font;
extern ImFont* bold_fontX16;
extern ImFont* icon_fontX19;
extern ImFont* icon_fontX12;
extern ImFont* cascadiacode_fontX14;