#include "imgui_custom.h"

bool ImCustom::SliderInt(const char* label, int* v, int v_min, int v_max, float slider_width, float slider_thickness, const char* display_format)
{
    bool SliderIntResult = false;
    ImVec2 sliderLinePos;
    ImVec2 currentCursorPos = ImGui::GetCursorPos();
    ImVec2 currentCursorScreenPos = ImGui::GetCursorScreenPos();
    sliderLinePos.x = currentCursorScreenPos.x;
    sliderLinePos.y = currentCursorScreenPos.y + 11 - slider_thickness/2;

    ImGui::SetCursorPos(currentCursorPos); // front line
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(sliderLinePos.x, sliderLinePos.y), ImVec2(currentCursorScreenPos.x + 2 + (slider_width-4) * (*v/static_cast<float>(v_max)), sliderLinePos.y + slider_thickness), ImGui::GetColorU32(ImGuiCol_SliderGrabActive), slider_thickness / 2);
    ImGui::SetCursorPos(currentCursorPos); // back line
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(sliderLinePos.x + 2 + (slider_width-4) *(*v/static_cast<float>(v_max)), sliderLinePos.y), ImVec2(currentCursorScreenPos.x + slider_width, sliderLinePos.y + slider_thickness), ImGui::GetColorU32(ImGuiCol_FrameBg), slider_thickness / 2);

    ImGui::SetCursorPos(currentCursorPos);
    ImGui::PushItemWidth(slider_width);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4.6f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0)); // invisible(transparent) slider bg
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImGui::GetStyleColorVec4(ImGuiCol_SliderGrab)); // disable color change on interaction
    //ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.6f, 0.6f, 0.6f, 1));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0)); // invisible value inside the slider
    SliderIntResult = ImGui::SliderInt(label, v, v_min, v_max, display_format);
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    return SliderIntResult;
}

bool ImCustom::InputInt(const char* label, int* v, char* text_buf, unsigned short max_size, float item_width, ImGuiInputTextFlags extra_flags)
{
    bool InputIntResult = false;
    ImGui::PushItemWidth(item_width);
    InputIntResult = ImGui::InputText(label, text_buf, max_size, extra_flags | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    try {
        *v = std::atoi(text_buf);
    }
    catch (std::exception&) {}

    return InputIntResult;
}

bool ImCustom::InputFloat(const char* label, float* v, char* text_buf, unsigned short max_size, float item_width, ImGuiInputTextFlags extra_flags)
{
    bool InputIntResult = false;
    ImGui::PushItemWidth(item_width);
    InputIntResult = ImGui::InputText(label, text_buf, max_size, extra_flags | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    try {
        *v = std::atof(text_buf);
    }
    catch (std::exception&) {}

    return InputIntResult;
}

void ImCustom::TextOutline(const char* text, ImVec4 outline_color, const float outline_size)
{
    ImU32 outline_colorU32 = ImGui::GetColorU32(outline_color);
    ImDrawList* drawlist = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    drawlist->AddText(ImVec2(pos.x - outline_size, pos.y), outline_colorU32, text);
    drawlist->AddText(ImVec2(pos.x + outline_size, pos.y), outline_colorU32, text);
    drawlist->AddText(ImVec2(pos.x, pos.y - outline_size), outline_colorU32, text);
    drawlist->AddText(ImVec2(pos.x, pos.y + outline_size), outline_colorU32, text);

    drawlist->AddText(ImVec2(pos.x - outline_size, pos.y - outline_size), outline_colorU32, text);
    drawlist->AddText(ImVec2(pos.x + outline_size, pos.y - outline_size), outline_colorU32, text);
    drawlist->AddText(ImVec2(pos.x - outline_size, pos.y + outline_size), outline_colorU32, text);
    drawlist->AddText(ImVec2(pos.x + outline_size, pos.y + outline_size), outline_colorU32, text);

    drawlist->AddText(pos, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), text);
}
