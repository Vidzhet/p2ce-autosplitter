#include "imgui_menu.h"
#include "imgui_font.h"

ImFont* def_fontX14;
ImFont* def_fontX11;
ImFont* bold_font;
ImFont* icon_fontX19;
ImFont* icon_fontX12;
ImFont* cascadiacode_fontX14;

void IMGUI_SETSTYLE()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO();

    style.Colors[ImGuiCol_CloseButton] = ImVec4(0, 0, 0, 0);  // Убираем фон круга
    style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0, 0, 0, 0);  // Убираем фон при наведении
    style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0, 0, 0, 0);  // Убираем фон при клике
    style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.8f);
    style.WindowBorderSize = 0;

    // Устанавливаем цвет заголовка окна
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1, 0.4f, 0, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1, 0.4f, 0, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.08f, 0.7f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);  // Цвет слайдера прокрутки
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // Цвет слайдера при наведении
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.4f, 0, 1);  // Цвет слайдера при активном (перетаскивании)
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0, 0, 0, 0);
    // Устанавливаем цвет кнопок
    style.Colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1, 0.5f, 0, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1, 0.5f, 0, 0.3f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.4f, 0, 1);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1, 0.4f, 0, 0.5f);

    style.GrabRounding = 4;
    style.FrameRounding = 4; // Радиус скругления углов

    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 13.0f, NULL);
    ImGui::GetIO().Fonts->Build();
    def_fontX14 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 14.0f);
    def_fontX11 = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibri.ttf", 11.0f);
    cascadiacode_fontX14 = io.Fonts->AddFontFromMemoryTTF(cascadiacode_font_arr, sizeof(cascadiacode_font_arr), 14.0f);
    ImFontConfig icon_font_cfg;
    icon_font_cfg.PixelSnapH = true;
    icon_font_cfg.FontDataOwnedByAtlas = false;
    bold_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Calibrib.ttf", 14.0f);
    icon_fontX19 = io.Fonts->AddFontFromMemoryTTF(icon_font_arr, sizeof(icon_font_arr), 19.0f);
    icon_fontX12 = io.Fonts->AddFontFromMemoryTTF(icon_font_arr, sizeof(icon_font_arr), 12.0f);

    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
}