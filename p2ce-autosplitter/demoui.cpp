#include "demoui.h"
#include "console.h"
#include "misc.h"
#include "dx11hook/DirectX11Hook.h"
#include "imgui_menu.h"

int demo_tick()
{
    return *reinterpret_cast<int*>(demoTickAddress);
}

int demo_maxtick()
{
    int clean_maxtick = *reinterpret_cast<int*>(clean_demoMaxTickAddress);
    return clean_maxtick < 0 ? *reinterpret_cast<int*>(extra_demoMaxTickAddress)*2 : clean_maxtick;
}

std::string demo_time()
{
    int seconds = *reinterpret_cast<int*>(demoTickAddress) / 60;
    int minutes = seconds > 59 ? seconds / 60 : 0;
    seconds = minutes > 0 ? seconds - minutes * 60 : seconds;
    return (minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes)) + ":" + (seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds));
}

std::string demo_maxtime()
{
    int seconds = demo_maxtick() / 60;
    int minutes = seconds > 59 ? seconds / 60 : 0;
    seconds = minutes > 0 ? seconds - minutes * 60 : seconds;
    return (minutes < 10 ? "0" + std::to_string(minutes) : std::to_string(minutes)) + ":" + (seconds < 10 ? "0" + std::to_string(seconds) : std::to_string(seconds));
}

void demo_nexttick()
{
    engine->ConsoleCommand(std::string("demo_gototick " + std::to_string(demo_tick() + 1)).c_str());
}

void demo_prevtick()
{
    engine->ConsoleCommand(std::string("demo_gototick " + std::to_string(demo_tick() - 3)).c_str());
}

void demo_forward()
{
    engine->ConsoleCommand(std::string("demo_gototick " + std::to_string(demo_tick() + 120)).c_str());
}

void demo_back()
{
    engine->ConsoleCommand(std::string("demo_gototick " + std::to_string(demo_tick() - 120)).c_str());
}

bool ccommand_valid()
{
    for (const std::string& buf : ccommand_ignore) {
        if (std::string(ccommand::getLastCommand(), buf.size()) == buf) {
            return false;
        }
    }
    return true;
}

std::string openfile_dem(const std::wstring& initialDir, const std::wstring& cut) {
    wchar_t filePath[MAX_PATH] = { 0 };

    DirectX11Hook::minimizeWindow();

    HWND mhwnd = GetMainWindowHwnd(GetCurrentProcessId());

    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = mhwnd;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"DEM Files (*.dem)\0*.dem\0";
    ofn.lpstrInitialDir = initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // modal window
    if (GetOpenFileNameW(&ofn) == TRUE) {
        std::wstring fullPath = filePath;

        if (fullPath.find(cut) == 0) {
            fullPath = fullPath.substr(cut.length());
        }

        ShowWindow(mhwnd, SW_RESTORE);
        return std::string(fullPath.begin(), fullPath.end()); // convert from wstr
    }
    ShowWindow(mhwnd, SW_RESTORE);
    return "Not loaded";
}

std::wstring openfolder(const std::wstring& initialDir) {
    DirectX11Hook::minimizeWindow();
    HWND mhwnd = GetMainWindowHwnd(GetCurrentProcessId());

    // Initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    std::wstring folderPath;

    // Create the File Open Dialog object
    IFileDialog* pFileDialog = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

    if (SUCCEEDED(hr)) {
        // Set options to select folders only
        DWORD options;
        hr = pFileDialog->GetOptions(&options);
        if (SUCCEEDED(hr)) {
            pFileDialog->SetOptions(options | FOS_PICKFOLDERS);
        }

        // Set the initial folder if specified
        if (!initialDir.empty()) {
            IShellItem* psiFolder = nullptr;
            hr = SHCreateItemFromParsingName(initialDir.c_str(), nullptr, IID_PPV_ARGS(&psiFolder));
            if (SUCCEEDED(hr)) {
                pFileDialog->SetFolder(psiFolder);
                psiFolder->Release();
            }
        }

        // Show the dialog
        hr = pFileDialog->Show(mhwnd);
        if (SUCCEEDED(hr)) {
            // Get the selected folder
            IShellItem* pItem = nullptr;
            hr = pFileDialog->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszPath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);

                if (SUCCEEDED(hr)) {
                    folderPath = pszPath;
                    CoTaskMemFree(pszPath);
                }
                pItem->Release();
            }
        }
        pFileDialog->Release();
    }

    CoUninitialize();
    ShowWindow(mhwnd, SW_RESTORE);
    return folderPath.empty() ? L"Not loaded" : folderPath;
}

std::string demo_getname(const std::string& demopath)
{
    auto pos = demopath.rfind("\\");

    if (pos != std::string::npos) {
        return demopath.substr(pos + 1);
    }

    // if "//" not found, return source str
    return demopath;
}

std::vector<std::string> demo_getlist(const std::wstring& folderpath, const std::wstring& cut) {
    std::vector<std::string> result;

    try {
        bool folder_exists = false;

        for (const auto& entry : std::filesystem::directory_iterator(folderpath)) {
            folder_exists = true;
            if (entry.is_regular_file() && entry.path().extension() == L".dem") {
                std::wstring fullPath = entry.path().wstring();

                // cut string
                if (fullPath.find(cut) == 0) {
                    fullPath = fullPath.substr(cut.length());
                }

                result.push_back(std::string(fullPath.begin(), fullPath.end()));
            }
        }

        if (!folder_exists) {
            throw std::runtime_error("folder not found");
        }
        if (result.empty()) {
            throw std::runtime_error(".dem files not found");
        }

        // sort by name
        std::sort(result.begin(), result.end(), compareFileNames);
    }
    catch (const std::filesystem::filesystem_error&) {
        throw std::runtime_error("folder not found");
    }

    return result;
}