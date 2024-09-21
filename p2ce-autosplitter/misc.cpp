#include "misc.h"

DWORD GetProcessID(const std::wstring& processName) {
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

uintptr_t GetModuleBaseAddress(DWORD processID, const std::wstring& moduleName) {
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

std::string getDemoPath() {
    std::filesystem::path exec_path = std::filesystem::current_path(); // Текущая рабочая директория
    std::cout << "Current path: " << exec_path << std::endl;

    // 2. Получаем текущее время
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_time = *std::localtime(&now_c);

    // 3. Формируем строку для папки с названием текущей даты и времени
    std::ostringstream folder_name;
    folder_name << std::put_time(&local_time, "%Y-%m-%d_%H-%M");

    // 4. Создаем папку по пути исполняемого файла
    std::filesystem::path new_folder = exec_path / folder_name.str();
    if (std::filesystem::create_directory(new_folder)) {
        std::cout << "Directory created: " << new_folder << std::endl;
    }
    else {
        std::cerr << "Failed to create directory." << std::endl;
    }
}
