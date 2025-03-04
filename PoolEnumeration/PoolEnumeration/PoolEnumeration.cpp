#include <windows.h>
#include <winternl.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>

typedef struct _SYSTEM_POOLTAG {
    union {
        UCHAR Tag[4];
        ULONG TagUlong;
    };
    ULONG PagedAllocs;
    ULONG PagedFrees;
    SIZE_T PagedUsed;
    ULONG NonPagedAllocs;
    ULONG NonPagedFrees;
    SIZE_T NonPagedUsed;
} SYSTEM_POOLTAG, * PSYSTEM_POOLTAG;

typedef struct _SYSTEM_POOLTAG_INFORMATION {
    ULONG Count;
    SYSTEM_POOLTAG TagInfo[1];
} SYSTEM_POOLTAG_INFORMATION, * PSYSTEM_POOLTAG_INFORMATION;

typedef NTSTATUS(NTAPI* pNtQuerySystemInformation)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

#define SystemPoolTagInformation 0x16
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

std::unordered_map<std::string, std::string> poolTagDescriptions;

void LoadPoolTagDatabase(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open " << filename << ". Using default unknown tags.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string tag, binary, description;

        if (!(ss >> tag >> binary)) continue;

        std::getline(ss, description);
        poolTagDescriptions[tag] = binary + " - " + description;
    }

    file.close();
}

void PrintBanner() {
    std::cout << "\n🚀 WRENCH IS HUNGRY FOR YOU BABY Love me all Night ! 🔥\n\n";
}

void PrintPoolTagInfo() {
    pNtQuerySystemInformation NtQuerySystemInformation =
        (pNtQuerySystemInformation)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");

    if (!NtQuerySystemInformation) {
        std::cerr << "[ERROR] Could not resolve NtQuerySystemInformation.\n";
        return;
    }

    ULONG bufferSize = 0x1000;
    PVOID buffer = nullptr;
    ULONG returnLength = 0;
    NTSTATUS status;

    do {
        if (buffer) VirtualFree(buffer, 0, MEM_RELEASE);
        buffer = VirtualAlloc(nullptr, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!buffer) {
            std::cerr << "[ERROR] Memory allocation failed.\n";
            return;
        }

        status = NtQuerySystemInformation(SystemPoolTagInformation, buffer, bufferSize, &returnLength);
        bufferSize *= 2;

    } while (status == STATUS_INFO_LENGTH_MISMATCH);

    if (!NT_SUCCESS(status)) {
        std::cerr << "[ERROR] Failed to query system pool tag information.\n";
        VirtualFree(buffer, 0, MEM_RELEASE);
        return;
    }

    PSYSTEM_POOLTAG_INFORMATION poolInfo = (PSYSTEM_POOLTAG_INFORMATION)buffer;

    std::cout << "Tag   Count   PagedBytes   NonPagedBytes   Description\n";
    for (ULONG i = 0; i < poolInfo->Count; i++) {
        SYSTEM_POOLTAG& tag = poolInfo->TagInfo[i];
        char tagString[5] = { 0 };
        memcpy(tagString, tag.Tag, 4);

        std::string description = "UNKNOWN pooltag '" + std::string(tagString) + "', please update pooltag.txt";
        if (poolTagDescriptions.find(tagString) != poolTagDescriptions.end()) {
            description = poolTagDescriptions[tagString];
        }

        printf("%-6s %6u %12llu %12llu  %s\n",
            tagString,
            tag.PagedAllocs + tag.NonPagedAllocs,
            tag.PagedUsed,
            tag.NonPagedUsed,
            description.c_str());
    }

    VirtualFree(buffer, 0, MEM_RELEASE);
}

int main() {
    LoadPoolTagDatabase("pooltag.txt");
    PrintBanner();
    PrintPoolTagInfo();
    return 0;
}
