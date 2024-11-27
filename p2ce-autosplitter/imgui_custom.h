#pragma once
#include "imgui_menu.h"

namespace ImCustom {
    // draw a value outside of this dogshit! (it hides a value by the default)
    bool SliderInt(const char* label, int* v, int v_min, int v_max, float slider_width = 100, float slider_thickness = 4, const char* display_format = "%.0f");
    // text_buf parameter should be char arr with initial size
    bool InputInt(const char* label, int* v, char* text_buf, unsigned short max_size, float item_width = 40, ImGuiInputTextFlags extra_flags = 0);
    bool InputFloat(const char* label, float* v, char* text_buf, unsigned short max_size, float item_width = 40, ImGuiInputTextFlags extra_flags = 0);
    // yeah i made that but it turns to be unused
    void TextOutline(const char* text, ImVec4 outline_color, const float outline_size = 1.0f);
}