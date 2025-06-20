#include "imgui_menu.h"
#include "imgui_custom.h"
#include "console.h"
#include "demoui.h"
#include "misc.h"
#include <optional>
#include "externs.h"

#define def_pos ImVec2(20, (ImGui::GetIO().DisplaySize.y - 200) / 2)

bool imgui_menu = false;
bool do_once = true;
bool apply_def_pos = true;
bool apply_def_size = true;
bool free_move = true;
bool free_resize = false;
bool autoplay_loaded_demo = true;
bool autoload_next_demo = true;
bool menu_collapsed = true;
bool collapse_menu = false;
bool switch_button_hovered = false;
bool input_speed_active = false;
float input_speed;
float slider_speed = 100;
char input_speed_textBuf[8] = "100.0";
int input_gototick;
int slider_tick = 0;
int slider_tickBuf;
std::string textinput;
bool settings_menu = false;
bool list_menu = false;
ImVec2 menu_sizeBuf = ImVec2(300, 200);
bool demo_paused = false;
bool slider_tick_active = false;
std::string last_ccommand("...");
std::string loaded_demo("Not loaded");
std::vector<std::string> demo_list;
std::optional<std::vector<std::string>::iterator> demo_list_iterator;
std::optional<std::vector<std::string>::iterator> demo_list_selected_iterator;
std::string list_folder("...");
std::string list_selected_file;
bool demolist_slideractive = false;
bool allign_list_loaded_file = false;
bool demo_loadnext = false;
bool demo_loadnext_allow = true;

ImGuiIO* io;
ImDrawList* imgui_drawlist;

void IMGUI_RENDER()
{
    if (do_once) {
        ImGui::SetNextWindowSize(ImVec2(300, 200));
        ImGui::SetNextWindowPos(def_pos);
        ImGui::SetNextWindowCollapsed(false);
        io = &ImGui::GetIO();
        imgui_drawlist = ImGui::GetOverlayDrawList();

        do_once = false;
    }

    // drawing livesplit connected or not in the corner
    if (engine->IsLevelMainMenuBackground() || engine->IsPaused()) {
        ImGui::PushFont(bold_fontX16);
        if (livesplit_connected) {
            ImVec2 green_text_size = ImGui::CalcTextSize("LiveSplit connected!");
            imgui_drawlist->AddText(ImVec2(io->DisplaySize.x - green_text_size.x - 10, 30), IM_COL32(92, 255, 122, 255), "LiveSplit connected!");
        }
        else {
            ImVec2 red_text_size = ImGui::CalcTextSize("LiveSplit is not connected!");
            imgui_drawlist->AddText(ImVec2(io->DisplaySize.x - red_text_size.x - 10, 30), IM_COL32(255, 92, 92, 255), "LiveSplit is not connected!");
        }
        ImGui::PopFont();
    }

    static bool timer_resume_apply = false;
    if (timer_resume && !timer_resume_apply) {
        timer_resume_apply = true;
        std::cout << "frame1\n";
    }
    else if (timer_resume_apply) {
        timer_resume_apply = timer_resume = false;
        std::cout << "frame2\n";
    }
    
    static bool ent_cvar_protect_apply = false;
    if (timer_resume && !ent_cvar_protect_apply) {
        ent_cvar_protect_apply = true;
        std::cout << "frame1\n";
    }
    else if (ent_cvar_protect_apply) {
        ent_cvar_protect_apply = timer_resume = false;
        g_pCVar->FindVar("ent_fire")->AddFlags(FCVAR_CHEAT);
        g_pCVar->FindVar("ent_create")->AddFlags(FCVAR_CHEAT);
        std::cout << "frame2\n";
    }


    if (apply_def_pos) {
        ImGui::SetNextWindowPos(def_pos);
        apply_def_pos = false;
    }
    if (apply_def_size) {
        ImGui::SetNextWindowSize(ImVec2(300, 200));
        menu_sizeBuf = ImVec2(300, 200);
        apply_def_size = false;
    }
    

    if (menu_collapsed && collapse_menu) {
        ImGui::SetNextWindowSize(ImVec2(menu_sizeBuf.x < 200 ? menu_sizeBuf.x : 200, 200));
        collapse_menu = false;
    }
    else if (!menu_collapsed && collapse_menu) {
        ImGui::SetNextWindowSize(menu_sizeBuf);
        collapse_menu = false;
    }

    static bool draw_cursor = true;
    if (imgui_menu && !draw_cursor && engine->IsPlayingDemo()) {
        ImGui::GetIO().MouseDrawCursor = true;
        draw_cursor = true;
    }
    else if (draw_cursor && !engine->IsPlayingDemo()) {
        ImGui::GetIO().MouseDrawCursor = false;
        draw_cursor = false;
    }

    static bool autoload_next_demo_allow = false;
    if (autoload_next_demo) { // autoload next demo
        if (autoload_next_demo_allow) { // checking IsDrawingLoadingImage() on the next frame
            if (!engine->IsDrawingLoadingImage() && !demo_list.empty() && demo_list_iterator.has_value() && **demo_list_iterator == loaded_demo && std::next(*demo_list_iterator) != demo_list.end()) {
                loaded_demo = *++ * demo_list_iterator;
                engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str());
                engine->ClientCmd_Unrestricted("ccommand_executed"); // hardcode fixzz
                allign_list_loaded_file = true;
                demo_paused = false;
                //std::cout << "demo_loadnext\n";
            }
            autoload_next_demo_allow = false;
        }
        if (demo_loadnext && !engine->IsPlayingDemo() && !engine->IsInGame()) {
            autoload_next_demo_allow = true;
            demo_loadnext = false;
        }
        else if (engine->IsPlayingDemo() && !demo_loadnext && engine->IsInGame()) { demo_loadnext = true; }
    }

    if (imgui_menu) {
        ImGui::Begin("Demo Control", &imgui_menu, ImGuiWindowFlags_NoScrollbar | (free_move ? 0 : ImGuiWindowFlags_NoMove) | (free_resize ? 0 : ImGuiWindowFlags_NoResize));

        if (!menu_collapsed && ImGui::IsWindowCollapsed()) {
            menu_collapsed = true;
            collapse_menu = true;
        }
        else if (menu_collapsed && !ImGui::IsWindowCollapsed()) {
            menu_collapsed = false;
            collapse_menu = true;
        }
        if (!ImGui::IsWindowCollapsed() && !collapse_menu) { menu_sizeBuf = ImGui::GetWindowSize(); }

        if (settings_menu && !ImGui::IsWindowCollapsed()) { // settings menu
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 200, ImGui::GetWindowPos().y - 120));
            ImGui::SetNextWindowSize(ImVec2(200, 120));
            ImGui::Begin("Settings", &settings_menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

            if (ImGui::Checkbox("Free move", &free_move)) {
                if (!free_move) {
                    apply_def_pos = true;
                }
            }
            if (ImGui::Checkbox("Free resize", &free_resize)) {
                if (!free_resize) {
                    apply_def_size = true;
                }
            }
            ImGui::Checkbox("Autoplay loaded demo", &autoplay_loaded_demo);
            ImGui::Checkbox("Autoload next demo in list", &autoload_next_demo);

            ImGui::End();
        }
        if (list_menu && !ImGui::IsWindowCollapsed()) { // list menu
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth() + 8, ImGui::GetWindowPos().y));
            ImGui::SetNextWindowSize(ImVec2(218, 300));
            ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 16);
            ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 32);
            ImGui::Begin("Demo List", &list_menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

            //ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100 -16);
            if (ImGui::Button("Open", ImVec2(62, 21))) {
                std::thread([]() { // executing winapi dialog window code in separate thread to avoid blocking swapchain(basically gamethread)
                    std::wstring folder_path = openfolder(gamePath + L"\\" + gameNameW);
                    try {
                        demo_list = demo_getlist(folder_path, gamePath + L"\\" + gameNameW + L"\\");
                        list_folder = demo_getname(std::string(folder_path.begin(), folder_path.end()));
                    }
                    catch (const std::runtime_error& ex) {
                        std::cout << ex.what() << std::endl;
                    }
                    }).detach();
            }
            ImGui::SameLine();
            //ImGui::SetCursorPosX(ImGui::GetWindowWidth()/2 - 28);
            if (ImGui::Button("Close", ImVec2(62, 21))) {
                demo_list.clear();
                list_folder = "...";
            }
            //ImGui::SameLine(ImGui::GetWindowWidth() - 56 -8);
            ImGui::SameLine();
            ImGui::Button("Select", ImVec2(62, 21));
            if (ImGui::IsItemClicked()) { // using isitemclicked instead of button callback bc it returns true on lmb down, default button callback activates on lmb up
                if (!list_selected_file.empty()) {
                    loaded_demo = list_selected_file;
                    if (demo_list_selected_iterator.has_value()) {
                        demo_list_iterator = demo_list_selected_iterator;
                    }
                    if (autoplay_loaded_demo) {
                        engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str());
                        demo_paused = false;
                    }
                }
            }
            else if (ImGui::IsMouseClicked(0) && !list_selected_file.empty()) { list_selected_file.clear(); } // deselect item

            ImGui::Dummy(ImVec2(0, 0)); // lol it just moves stuff down for a bit
            ImGui::Separator();

            ImGui::Text(("Folder: " + list_folder).c_str());

            ImGui::Separator();

            ImGui::BeginChild("##list", ImVec2(208, 210));

            if (!demo_list.empty()) {
                std::string file;
                for (auto it = demo_list.begin(); it != demo_list.end(); ++it) { // replaced for each with iterator
                    file = *it;
                    if (list_selected_file == file && file != loaded_demo) {
                        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 2), ImVec2(ImGui::GetCursorScreenPos().x + 200, ImGui::GetCursorScreenPos().y + 16), ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.2f)));
                        //ImCustom::TextOutline(demo_getname(file).c_str(), ImGui::GetStyleColorVec4(ImGuiCol_CheckMark), 0.1f);
                        ImGui::SetCursorPosX(0);
                    }
                    if (ImGui::InvisibleButton(file.c_str(), ImVec2(202, 14))) {
                        list_selected_file = file;
                        demo_list_selected_iterator = it;
                    }
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {  // 0 = lmb
                        loaded_demo = file;
                        demo_list_iterator = it;
                        list_selected_file.clear();
                        if (autoplay_loaded_demo) { 
                            engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str()); 
                            demo_paused = false;
                        }
                    }

                    ImGui::SameLine();
                    ImGui::SetCursorPosX(0);
                    if (file == loaded_demo) {
                        ImGui::PushFont(bold_font);
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
                        ImGui::TextColored(ImGui::IsItemHovered() ? ImGui::GetStyleColorVec4(ImGuiCol_CheckMark) : ImGui::GetStyleColorVec4(ImGuiCol_Text), ("> " + demo_getname(file)).c_str());
                        ImGui::PopFont();
                        if (allign_list_loaded_file) {
                            // if loaded file out of visible window
                            if (ImGui::GetCursorPosY() < ImGui::GetScrollY() + ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().WindowPadding.y) { // if above visible window
                                ImGui::SetScrollY(ImGui::GetCursorPosY() - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
                            }
                            else if (ImGui::GetCursorPosY() > ImGui::GetScrollY() + ImGui::GetWindowHeight()) { // if below visible window
                                ImGui::SetScrollY(ImGui::GetCursorPosY() - ImGui::GetWindowHeight() + ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().WindowPadding.y - 18);
                            }
                            allign_list_loaded_file = false;
                        }
                    }
                    else {
                        ImGui::TextColored(ImGui::IsItemHovered() ? ImGui::GetStyleColorVec4(ImGuiCol_CheckMark) : ImGui::GetStyleColorVec4(ImGuiCol_Text), demo_getname(file).c_str());
                    }
                }
            }

            ImGui::EndChild();
            ImGui::End();
            ImGui::PopStyleVar(2);
        }

        if (engine->IsPlayingDemo() && ccommand_valid()) {
            last_ccommand = ccommand::getLastCommand();
        }

        if (ImGui::Button("Load", ImVec2(50, 21))) {
            std::thread([]() { // executing winapi dialog window code in separate thread to avoid blocking swapchain(basically gamethread)
                std::string temp = openfile_dem(gamePath + L"\\" + gameNameW, gamePath + L"\\" + gameNameW + L"\\");
                if (temp != "Not loaded") {
                    loaded_demo = temp;
                    if (autoplay_loaded_demo) {
                        engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str());
                        demo_paused = false;
                    }
                }
                }).detach();
        }
        ImGui::SameLine();
        if (list_menu) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
            if (ImGui::Button("List", ImVec2(50, 21))) {
                list_menu = !list_menu;
            }
            ImGui::PopStyleColor();
        }
        else {
            if (ImGui::Button("List", ImVec2(50, 21))) {
                list_menu = !list_menu;
            }
        }
        ImGui::SameLine(ImGui::GetWindowWidth() - 87);
        if (ImGui::Button("##settings_button", ImVec2(21, 21))) { // settings button
            settings_menu = !settings_menu;
        }
        ImGui::SameLine(ImGui::GetWindowWidth() - 58);
        if (ImGui::Button("Stop", ImVec2(50, 21))) {
            engine->ClientCmd_Unrestricted("stopdemo");
        }
        ImGui::SameLine(ImGui::GetWindowWidth() - 83);
        ImGui::PushFont(icon_fontX19);
        ImGui::TextColored(settings_menu ? ImGui::GetStyleColorVec4(ImGuiCol_CheckMark) : ImGui::GetStyleColorVec4(ImGuiCol_Text), "+");
        ImGui::PopFont();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
        if (ImGui::Button("Goto:", ImVec2(56, 21)) && engine->IsPlayingDemo()) {
            engine->ClientCmd_Unrestricted(std::string("demo_gototick " + std::to_string(input_gototick)).c_str());
            engine->ClientCmd_Unrestricted(demo_paused ? "demo_pause" : "demo_resume");
        }
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 12, ImGui::GetCursorPosY() + 1)); // create a square
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 1), ImVec2(ImGui::GetCursorScreenPos().x + 24, ImGui::GetCursorScreenPos().y + 20), ImGui::GetColorU32(ImGuiCol_FrameBg));
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPos().x - 12);
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y), ImVec2(ImGui::GetCursorScreenPos().x + 4, ImGui::GetCursorScreenPos().y + 21), ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f)));
        ImGui::SameLine(ImGui::GetCursorPosX() + 12);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4)); // edit the height
        static char input_gototick_textBuf[6];
        if (ImCustom::InputInt("##input_gototick", &input_gototick, input_gototick_textBuf, 6, 44, ImGuiInputTextFlags_EnterReturnsTrue) && engine->IsPlayingDemo()) {
            engine->ClientCmd_Unrestricted(std::string("demo_gototick " + std::to_string(input_gototick)).c_str());
            engine->ClientCmd_Unrestricted(demo_paused ? "demo_pause" : "demo_resume");
        }
        ImGui::PopStyleVar();
        /*ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::CalcTextSize("Speed:").x + 94 + 48));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);
        ImGui::Text("Speed:");*/
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (61)); // create a square
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 2), ImVec2(ImGui::GetCursorScreenPos().x + 18, ImGui::GetCursorScreenPos().y + 19), ImGui::GetColorU32(ImGuiCol_FrameBg));
        ImGui::SameLine();
        //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
        ImGui::PushItemWidth(80);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (88 + 40 + 8 + 1)); // slider pos
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4)); // edit the height
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 0)); // make value inside the slider invisible
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.7f, 0.4f, 0, 1));
        if (ImGui::SliderFloat("##slider_speed", &input_speed, 1, 400, "%.1f")) {
            engine->ClientCmd_Unrestricted(std::string("demo_timescale " + std::to_string(input_speed / 100)).c_str());
            std::strcpy(input_speed_textBuf, float_tostr(input_speed).c_str());
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 48);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4)); // edit the height
        ImCustom::InputFloat("##input_speed", &input_speed, input_speed_textBuf, 6, 40);
        if (ImGui::IsItemActive()) {
            engine->ClientCmd_Unrestricted(std::string("demo_timescale " + std::to_string(input_speed / 100)).c_str());
            input_speed_active = true;
        }
        else if (input_speed_active) {
            std::strcpy(input_speed_textBuf, float_tostr(input_speed).c_str());
            input_speed_active = false;
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 57); // create a line in front
        ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + 4, ImGui::GetCursorScreenPos().y + 21), ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f)));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 48 + ImGui::CalcTextSize(input_speed_textBuf).x +1, ImGui::GetCursorPosY() + 2)); //
        ImGui::PushFont(def_fontX11);
        ImGui::Text("%%");
        ImGui::PopFont();

        //ImGui::NewLine();

        ImGui::Dummy(ImVec2(0, 1));
        ImGui::Text(("Demo: " + demo_getname(loaded_demo)).c_str());
        ImGui::SameLine();
        ImGui::Text(std::string("(" + demo_maxtime() + ")").c_str());
        ImCustom::SliderInt("##slider_tick", &slider_tick, 1, demo_maxtick() < 1 ? 1000 : demo_maxtick(), ImGui::GetWindowWidth() - (ImGui::CalcTextSize(demo_time().c_str()).x + 36), 6);
        if (ImGui::IsItemActive() && engine->IsPlayingDemo()) 
        {
            engine->ClientCmd_Unrestricted(std::string("demo_goto " + std::to_string(slider_tick)).c_str());
            engine->ClientCmd_Unrestricted(demo_paused ? "demo_pause" : "demo_resume");
        }
        else {
            slider_tick = demo_tick() < 0 ? 0 : demo_tick();
        }
        ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::CalcTextSize(demo_time().c_str()).x + 18));
        ImGui::PushFont(def_fontX14);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
        ImGui::Text(demo_time().c_str());
        ImGui::PopFont();

        //std::cout << demo_time() << " / " << demo_maxtime << std::endl;

        ImGui::Dummy(ImVec2(0, 1));
        if (ImGui::Button("Prev", ImVec2(50, 22))) {
            if (!demo_list.empty() && demo_list_iterator.has_value() && **demo_list_iterator == loaded_demo && *demo_list_iterator != demo_list.begin()) {
                loaded_demo = *--*demo_list_iterator;
                engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str());
                allign_list_loaded_file = true;
                demo_paused = false;
            }
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8 + (ImGui::GetWindowWidth() - 16 - 250) / 6);
        if (ImGui::Button("<<", ImVec2(25, 22))) {
            demo_back();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8 + (ImGui::GetWindowWidth() - 16 - 250) / 6);
        if (ImGui::Button("<", ImVec2(25, 22))) {
            demo_prevtick();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8 + (ImGui::GetWindowWidth() - 16 - 250) / 6);
        if (ImGui::Button(!engine->IsPlayingDemo() ? "Play" : demo_paused ? "Resume" : "Pause", ImVec2(50, 22))) {
            if (engine->IsPlayingDemo()) {
                engine->ClientCmd_Unrestricted("demo_togglepause");
                demo_paused = !demo_paused;
            }
            else if (loaded_demo != "Not loaded") { // Play button
                engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str());
                demo_paused = false;
            }
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8 + (ImGui::GetWindowWidth() - 16 - 250) / 6);
        if (ImGui::Button(">", ImVec2(25, 22))) {
            demo_nexttick();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8 + (ImGui::GetWindowWidth() - 16 - 250) / 6);
        if (ImGui::Button(">>", ImVec2(25, 22))) {
            demo_forward();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth()- 8 -50);
        if (ImGui::Button("Next", ImVec2(50, 22))) {
            if (!demo_list.empty() && demo_list_iterator.has_value() && **demo_list_iterator == loaded_demo && std::next(*demo_list_iterator) != demo_list.end()) {
                loaded_demo =*++*demo_list_iterator; // holy fuck what the hell is that =*++* I swear, i will never touch iterators again
                engine->ClientCmd_Unrestricted(("playdemo " + loaded_demo).c_str());
                allign_list_loaded_file = true;
                demo_paused = false;
            }
        }

        ImGui::Dummy(ImVec2(0, 1));
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() - 1));
        ImGui::PushFont(def_fontX14);
        ImGui::Text("Tick:");
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 4, ImGui::GetCursorPosY() + 1));
        ImGui::Text((std::to_string(demo_tick()) + " / " + std::to_string(demo_maxtick())).c_str());

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
        ImGui::Text("/>");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2);
        ImGui::Text(last_ccommand.c_str());

        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 24 - ImGui::CalcTextSize("Switch to legacyui @").x, ImGui::GetWindowHeight() - 20));
        if (!free_resize) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6); }
        ImGui::PushFont(bold_font);
        ImGui::TextColored(switch_button_hovered ? ImGui::GetStyleColorVec4(ImGuiCol_CheckMark) : ImGui::GetStyleColorVec4(ImGuiCol_Text), "Switch to legacyui    "); // extra spacing to make text hitbox bigger
        ImGui::PopFont();
        if (ImGui::IsItemHovered()) { switch_button_hovered = true; }
        else { switch_button_hovered = false; }
        if (ImGui::IsItemClicked()) { // on text click event
            imgui_menu = false;
            engine->ClientCmd_Unrestricted("legacy_demoui");
        }
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 24, ImGui::GetWindowHeight()-20));
        if (!free_resize) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6); }
        ImGui::PushFont(icon_fontX12);
        ImGui::TextColored(switch_button_hovered ? ImGui::GetStyleColorVec4(ImGuiCol_CheckMark) : ImGui::GetStyleColorVec4(ImGuiCol_Text), "!");
        ImGui::PopFont();

        ImGui::End();
    }
}