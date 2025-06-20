#include "misc.h"
#include "externs.h"
#include <fstream>

DWORD GetProcessID(const std::string& processName) {
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &processEntry)) {
        do {
            if (processName == processEntry.szExeFile) {
                DWORD processID = processEntry.th32ProcessID;
                CloseHandle(snapshot);
                return processID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

uintptr_t GetModuleBaseAddress(DWORD processID, const std::string& moduleName) {
    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (moduleName == moduleEntry.szModule) {
                uintptr_t baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                CloseHandle(snapshot);
                return baseAddress;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

uintptr_t ResolvePointerChain(HANDLE processHandle, uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) {
    uintptr_t currentAddress = baseAddress;
    for (const auto& offset : offsets) {
        if (!ReadProcessMemory(processHandle, (LPCVOID)currentAddress, &currentAddress, sizeof(currentAddress), nullptr)) {
            return 0;
        }
        currentAddress += offset;
    }
    return currentAddress;
}
uintptr_t memory::ptrOffset(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) {
    for (const auto& offset : offsets) {
        baseAddress = *reinterpret_cast<uintptr_t*>(baseAddress) + offset;
    }
    return baseAddress;
}

std::wstring getGamePath() {
    wchar_t gamePath[MAX_PATH];
    GetFullPathNameW(L"bin\\win64\\p2ce-autosplitter.dll", MAX_PATH, (LPWSTR)&gamePath, nullptr);
    gamePath[wcslen(gamePath) - wcslen(L"\\bin\\win64\\p2ce-autosplitter.dll")] = '\0';

    return std::wstring(gamePath);
}

std::wstring getCurrentTimeW() { // code from stackoverflow lol
    // getting current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm now_tm;
    // localtime_s instead of localtime
    localtime_s(&now_tm, &now_c);

    // formatting time
    std::wostringstream oss;
    oss << std::setw(4) << std::setfill(L'0') << (now_tm.tm_year + 1900) << L"-"   // year
        << std::setw(2) << std::setfill(L'0') << (now_tm.tm_mon + 1) << L"-"       // month
        << std::setw(2) << std::setfill(L'0') << now_tm.tm_mday << L"_"            // date
        << std::setw(2) << std::setfill(L'0') << now_tm.tm_hour << L"-"            // hours
        << std::setw(2) << std::setfill(L'0') << now_tm.tm_min;                    // minutes

    return oss.str();
}

std::string float_tostr(float value) { // code from stackoverflow ...again
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

bool compareFileNames(const std::string& a, const std::string& b) {
    auto splitParts = [](const std::string& filename) {
        std::vector<std::string> parts;
        std::string current;

        for (char ch : filename) {
            if (std::isdigit(ch)) {
                if (!current.empty() && !std::isdigit(current.back())) {
                    parts.push_back(current);
                    current.clear();
                }
                current += ch;
            }
            else {
                if (!current.empty() && std::isdigit(current.back())) {
                    parts.push_back(current);
                    current.clear();
                }
                current += ch;
            }
        }

        if (!current.empty()) {
            parts.push_back(current);
        }

        return parts;
        };

    std::vector<std::string> a_parts = splitParts(a);
    std::vector<std::string> b_parts = splitParts(b);

    size_t i = 0;
    while (i < a_parts.size() && i < b_parts.size()) {
        if (std::all_of(a_parts[i].begin(), a_parts[i].end(), ::isdigit) &&
            std::all_of(b_parts[i].begin(), b_parts[i].end(), ::isdigit)) {
            int num_a = std::stoi(a_parts[i]);
            int num_b = std::stoi(b_parts[i]);
            if (num_a != num_b) {
                return num_a < num_b;
            }
        }
        else {
            if (a_parts[i] != b_parts[i]) {
                return a_parts[i] < b_parts[i];
            }
        }
        ++i;
    }

    return a_parts.size() < b_parts.size();
}

void LoadSpeedrunConfig(const wchar_t* file) {
    std::wstring tmp(file);
    std::wifstream in(tmp);

    if (!in.is_open()) return;

    std::wstring line;
    bool in_end_section = false;

    while (std::getline(in, line)) {
        // Trim
        line.erase(0, line.find_first_not_of(L" \t\r\n"));
        line.erase(line.find_last_not_of(L" \t\r\n") + 1);

        if (line.empty() || line[0] == L'#' || line[0] == L';')
            continue;

        if (line == L"[End]") {
            in_end_section = true;
            continue;
        }

        if (in_end_section) {
            if (line[0] == L'[') break;

            if (line.find(L"map=") == 0) {
                speedrun_endMap = std::string(line.begin() + 4, line.end());
            }
            else if (line.find(L"entity=") == 0) {
                speedrun_endInputEntity = std::string(line.begin() + 7, line.end());
            }
            else if (line.find(L"input=") == 0) {
                speedrun_endInput = std::string(line.begin() + 6, line.end());
            }
        }
    }
}


char* memory::find_str(const char* moduleName, const std::string& searchString) {
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) {
        return nullptr;
    }
    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
        return nullptr;
    }

    char* start = reinterpret_cast<char*>(modInfo.lpBaseOfDll); // start of module
    char* end = start + modInfo.SizeOfImage; // end of module

    for (char* p = start; p < end - searchString.size(); ++p) {
        if (memcmp(p, searchString.c_str(), searchString.size()) == 0) {
            return p;
        }
    }

    return nullptr;
}

char* memory::find_const_str(const char* moduleName, const std::string& searchString) {
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) {
        return nullptr;
    }
    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
        return nullptr;
    }

    char* start = reinterpret_cast<char*>(modInfo.lpBaseOfDll); // Начало модуля
    char* end = start + modInfo.SizeOfImage; // Конец модуля

    MEMORY_BASIC_INFORMATION mbi;
    for (char* p = start; p < end - searchString.size(); ++p) {
        if (memcmp(p, searchString.c_str(), searchString.size()) == 0) {
            // Проверяем, находится ли найденная строка в статическом сегменте (.data, .rdata)
            if (VirtualQuery(p, &mbi, sizeof(mbi))) {
                if (!(mbi.State & MEM_COMMIT) || (mbi.Protect & PAGE_GUARD)) {
                    continue; // Пропускаем невыделенную память
                }
                // Исключаем динамическую память (heap, stack)
                if (mbi.Type == MEM_IMAGE) {
                    return p; // Найденный адрес в исполняемом модуле (статическая память)
                }
            }
        }
    }
    return nullptr;
}
