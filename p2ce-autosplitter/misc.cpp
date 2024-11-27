#include "misc.h"

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
uintptr_t ptrOffset(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) {
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
    // Функция для разбивки строки на части (цифры и текст)
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
            parts.push_back(current); // добавляем последнюю часть
        }

        return parts;
        };

    // Разбиваем обе строки на части
    std::vector<std::string> a_parts = splitParts(a);
    std::vector<std::string> b_parts = splitParts(b);

    // Сравниваем части поочередно
    size_t i = 0;
    while (i < a_parts.size() && i < b_parts.size()) {
        // Сравниваем по числовым значениям, если часть числа
        if (std::all_of(a_parts[i].begin(), a_parts[i].end(), ::isdigit) &&
            std::all_of(b_parts[i].begin(), b_parts[i].end(), ::isdigit)) {
            int num_a = std::stoi(a_parts[i]);
            int num_b = std::stoi(b_parts[i]);
            if (num_a != num_b) {
                return num_a < num_b;
            }
        }
        else {
            // Сравниваем текстовые части
            if (a_parts[i] != b_parts[i]) {
                return a_parts[i] < b_parts[i];
            }
        }
        ++i;
    }

    // Если один из векторов длиннее, то он должен быть после
    return a_parts.size() < b_parts.size();
}