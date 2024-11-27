#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <wininet.h>
#include <commdlg.h>
#include <shlobj.h>
#include <cctype>
#include <conio.h>
#include <vector>
#include <sstream>
#include <algorithm>

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
bool download_queue(std::string rawfile, std::wstring dir, int count);
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

    // count how many files in update list
    int count = 0;
    std::istringstream countStream(fileContent);
    std::string templine;
    while (std::getline(countStream, templine)) {
        if (!templine.empty()) {
            ++count;
        }
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
        if (!download_queue(line.substr(0, spacePos), dir + L"\\" + std::wstring(line.begin() + spacePos + 1, line.end()), count)) {
            return false;
        }
    }

    return true;
}

std::wstring openfolder() {
    // Initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    std::wstring folderPath;

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
        IShellItem* psiFolder = nullptr;
        hr = SHCreateItemFromParsingName(nullptr, nullptr, IID_PPV_ARGS(&psiFolder));
        if (SUCCEEDED(hr)) {
            pFileDialog->SetFolder(psiFolder);
            psiFolder->Release();
        }

        // Show the dialog
        hr = pFileDialog->Show(nullptr);
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
    return folderPath.empty() ? L"Not loaded" : folderPath;
}
float download_queue_count = 1;
bool download_queue(std::string rawfile, std::wstring dir, int count) {
    int percent = download_queue_count / count * 20;
    std::cout << std::to_string(percent) << std::endl;
    std::string filename(rawfile.substr(rawfile.find_last_of('/') + 1));
    system("cls");
    std::cout << "Reading " << "\"" << filename << "\"" << "...\n" << std::endl;
    std::cout << "Downloading content:\t [" << std::string(percent, '|').c_str() << std::string(20 - percent, '.') << "] " << percent*5 << "%";
    if (!DownloadFile(rawfile, std::wstring(dir + L"\\" + std::wstring(filename.begin(), filename.end())))) {
        return false;
    }
    download_queue_count++;
    return true;
}

std::string demo_getname(const std::wstring& demopath) // lol i just bring this function here lolol (i dont care)
{
    auto pos = demopath.rfind(L"\\");

    if (pos != std::wstring::npos) {
        std::wstring result(demopath.substr(pos + 1));
        std::replace(result.begin(), result.end(), ' ', '_');
        return std::string(result.begin(), result.end());
    }

    return "ERROR BLYAD!!1!11!!!";
}

int main() {
    std::cout << "Welcome to p2ce-autosplitter installer!\n";
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    std::wstring initialdir = openfolder();

    if (process_update("https://raw.githubusercontent.com/Vidzhet/p2ce-autosplitter/refs/heads/master/update/" + demo_getname(initialdir) + "/update.txt", initialdir)) {
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
        std::cout << std::endl << "\nInstallation finished!";
    }
    else {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
        std::cout << std::endl << "\nInstallation failed.";
    }
    
    _getch(); // wait for input to exit
    return 0;
}
