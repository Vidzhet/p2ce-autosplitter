#pragma once
#include "dx11hook/dx11hook.h"

#define IMGUI_SETSTYLE() imgui_setstyle()
#define IMGUI_RENDER() imgui_render()

extern bool imgui_menu;
extern bool demo_mode;
extern bool demo_paused;

void IMGUI_SETSTYLE();
void IMGUI_RENDER();

extern ImFont* def_fontX14;
extern ImFont* def_fontX11;
extern ImFont* bold_font;
extern ImFont* icon_fontX19;
extern ImFont* icon_fontX12;
extern ImFont* cascadiacode_fontX14;