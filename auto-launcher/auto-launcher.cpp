#include <windows.h>
#include <iostream>
#include <string>
#include <wininet.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>

bool DownloadFile(const std::string& url, const std::wstring& destPath) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[4096];

    // init WinINet
    hInternet = InternetOpen(L"File Downloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "Error opening internet connection." << std::endl;
        return false;
    }

    hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "Error opening URL." << std::endl;
        InternetCloseHandle(hInternet);
        return false;
    }

    // open file
    std::ofstream file(destPath, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing." << std::endl;
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    // read and then write
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        file.write(buffer, bytesRead);
    }

    file.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return true;
}

bool process_update(const std::string& url, std::wstring dir) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[4096];
    std::string fileContent;

    // init WinINet
    hInternet = InternetOpen(L"File Downloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "Error opening internet connection." << std::endl;
        return false;
    }

    hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "Error opening URL." << std::endl;
        InternetCloseHandle(hInternet);
        return false;
    }

    // write received data to str
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        fileContent.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (fileContent.empty()) {
        std::cerr << "Error reading file content or file is empty." << std::endl;
        return false;
    }

    std::istringstream stream(fileContent);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        // split line to rawfile url and destination path
        size_t spacePos = line.find(' ');
        if (spacePos == std::string::npos) {
            std::cerr << "Invalid line format: " << line << std::endl;
            continue;
        }
        std::string rawfile(line.substr(0, spacePos));
        std::string filename(rawfile.substr(rawfile.find_last_of('/') + 1));
        std::wcout << L"download " << dir + L"\\" + std::wstring(line.begin() + spacePos + 1, line.end()) + L"\\" + std::wstring(filename.begin(), filename.end()) << std::endl;
        DownloadFile(rawfile, dir + L"\\" + std::wstring(line.begin() + spacePos + 1, line.end()) + L"\\" + std::wstring(filename.begin(), filename.end()));
    }

    return true;
}

std::string github_readversion(const std::string& url) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[4096];
    std::string fileContent;

    hInternet = InternetOpen(L"File Downloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "Error opening internet connection." << std::endl;
        return "";
    }

    hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "Error opening URL." << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        fileContent.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (fileContent.empty()) {
        std::cerr << "Error reading file content or file is empty." << std::endl;
    }

    fileContent.erase(fileContent.find_last_not_of("\r\n") + 1); // github adds escape sequence at the end of every line
    return fileContent;
}

std::string getFileVersion(const std::wstring& filePath) {
    // get size of data
    DWORD dummy;
    DWORD size = GetFileVersionInfoSizeW(filePath.c_str(), &dummy);

    if (size == 0) {
        std::wcout << L"unable to read file " << filePath << std::endl;
        return "";
    }

    // version str buffer
    BYTE* versionData = new BYTE[size];

    if (!GetFileVersionInfoW(filePath.c_str(), 0, size, versionData)) {
        std::wcout << L"unable to read info " << filePath << std::endl;
        delete[] versionData;
        return "";
    }

    // get version info
    VS_FIXEDFILEINFO* fileInfo;
    UINT fileInfoSize;

    if (VerQueryValueW(versionData, L"\\", (LPVOID*)&fileInfo, &fileInfoSize)) {
        delete[] versionData;
        return std::string(std::to_string(HIWORD(fileInfo->dwFileVersionMS)) + "." +
            std::to_string(LOWORD(fileInfo->dwFileVersionMS)) + "." +
            std::to_string(HIWORD(fileInfo->dwFileVersionLS)) + "." +
            std::to_string(LOWORD(fileInfo->dwFileVersionLS)));
    }
    else {
        std::cout << "failed to read version" << std::endl;
        delete[] versionData;
        return "";
    }
}

std::string demo_getname(const std::wstring& demopath) // lol i just brought this function here
{
    auto pos = demopath.rfind(L"\\");

    if (pos != std::wstring::npos) {
        std::wstring result(demopath.substr(pos + 1));
        std::replace(result.begin(), result.end(), ' ', '_');
        return std::string(result.begin(), result.end());
    }

    return "ERROR BLYAD!!1!11!!!";
}

void Inject() {
    wchar_t DllPath[MAX_PATH];
    GetFullPathNameW(L"bin\\win64\\p2ce-autosplitter.dll", MAX_PATH, (LPWSTR)&DllPath, nullptr); // originally returns Root derictory.
    LoadLibraryW(DllPath);
}
bool updated = false;
void Update() {
    wchar_t DllPath[MAX_PATH];
    GetFullPathNameW(L"bin\\win64\\p2ce-autosplitter.dll", MAX_PATH, (LPWSTR)&DllPath, nullptr);
    std::wstring RootPath = DllPath;
    RootPath.erase(RootPath.find(L"\\bin\\win64\\p2ce-autosplitter.dll"), 32);
    // check for updates
    //MessageBoxA(nullptr, (getFileVersion(DllPath) + github_readversion("https://raw.githubusercontent.com/Vidzhet/p2ce-autosplitter/refs/heads/master/update/version.txt")).c_str(), "...", MB_OK);
    if (getFileVersion(DllPath) != github_readversion("https://raw.githubusercontent.com/Vidzhet/p2ce-autosplitter/refs/heads/master/update/version.txt")) {
        process_update("https://raw.githubusercontent.com/Vidzhet/p2ce-autosplitter/refs/heads/master/update/" + demo_getname(RootPath) + "/update.txt", RootPath);
        CreateThread(nullptr, 0, [](LPVOID) -> DWORD { MessageBoxA(nullptr, "p2ce-autosplitter has been updated.\nYou may need to restart the game", "p2ce-autosplitter updated", MB_OK); return 0; }, nullptr, 0, nullptr);
    }
    updated = true;
}

_declspec(dllexport) void Load() {
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&Update, nullptr, 0, nullptr); // i dont know why i cant check for updates in main thread, it just doesnt work, i tried
    while (!updated) { Sleep(100); }
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&Inject, nullptr, 0, nullptr);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&Load, nullptr, 0, nullptr);
    }
    return TRUE;
}