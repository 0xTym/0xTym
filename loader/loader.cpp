#include "loader.h"
#include <TlHelp32.h>
#include <iostream>
#include <fstream>
#include <vector>

namespace Loader {

    // Shellcode for loading DLL in remote process
    #pragma pack(push, 1)
    struct LoaderData {
        LPVOID imageBase;
        PIMAGE_NT_HEADERS ntHeaders;
        PIMAGE_BASE_RELOCATION baseReloc;
        PIMAGE_IMPORT_DESCRIPTOR importDesc;

        // Function pointers (resolved in target process)
        decltype(&LoadLibraryA) fnLoadLibraryA;
        decltype(&GetProcAddress) fnGetProcAddress;
        decltype(&VirtualProtect) fnVirtualProtect;
        decltype(&DllMain) fnDllMain;
    };
    #pragma pack(pop)

    // Shellcode to execute in remote process
    DWORD WINAPI LoaderShellcode(LoaderData* pData) {
        if (!pData) return 1;

        PIMAGE_NT_HEADERS pNtHeaders = pData->ntHeaders;
        PIMAGE_BASE_RELOCATION pBaseReloc = pData->baseReloc;
        PIMAGE_IMPORT_DESCRIPTOR pImportDesc = pData->importDesc;

        // Fix relocations
        if (pBaseReloc && pBaseReloc->VirtualAddress) {
            DWORD delta = (DWORD)((LPBYTE)pData->imageBase - pNtHeaders->OptionalHeader.ImageBase);

            while (pBaseReloc->VirtualAddress) {
                if (pBaseReloc->SizeOfBlock >= sizeof(IMAGE_BASE_RELOCATION)) {
                    int count = (pBaseReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
                    PWORD list = (PWORD)(pBaseReloc + 1);

                    for (int i = 0; i < count; i++) {
                        if (list[i]) {
                            PDWORD ptr = (PDWORD)((LPBYTE)pData->imageBase + (pBaseReloc->VirtualAddress + (list[i] & 0xFFF)));
                            *ptr += delta;
                        }
                    }
                }

                pBaseReloc = (PIMAGE_BASE_RELOCATION)((LPBYTE)pBaseReloc + pBaseReloc->SizeOfBlock);
            }
        }

        // Resolve imports
        if (pImportDesc && pImportDesc->Characteristics) {
            while (pImportDesc->Characteristics) {
                PIMAGE_THUNK_DATA pOrigFirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)pData->imageBase + pImportDesc->OriginalFirstThunk);
                PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)pData->imageBase + pImportDesc->FirstThunk);

                HMODULE hModule = pData->fnLoadLibraryA((LPCSTR)pData->imageBase + pImportDesc->Name);

                if (!hModule) {
                    return 2;
                }

                while (pOrigFirstThunk->u1.AddressOfData) {
                    if (pOrigFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                        DWORD functionOrdinal = pOrigFirstThunk->u1.Ordinal & 0xFFFF;
                        pFirstThunk->u1.Function = (DWORD_PTR)pData->fnGetProcAddress(hModule, (LPCSTR)functionOrdinal);
                    } else {
                        PIMAGE_IMPORT_BY_NAME pImport = (PIMAGE_IMPORT_BY_NAME)((LPBYTE)pData->imageBase + pOrigFirstThunk->u1.AddressOfData);
                        pFirstThunk->u1.Function = (DWORD_PTR)pData->fnGetProcAddress(hModule, pImport->Name);
                    }

                    pOrigFirstThunk++;
                    pFirstThunk++;
                }

                pImportDesc++;
            }
        }

        // Set memory protections
        PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
        for (WORD i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++) {
            if (pSectionHeader->SizeOfRawData) {
                DWORD protect = PAGE_READONLY;

                if (pSectionHeader->Characteristics & IMAGE_SCN_MEM_WRITE) {
                    protect = PAGE_READWRITE;
                }
                if (pSectionHeader->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
                    protect = (protect == PAGE_READWRITE) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
                }

                DWORD oldProtect;
                pData->fnVirtualProtect((LPBYTE)pData->imageBase + pSectionHeader->VirtualAddress,
                                       pSectionHeader->SizeOfRawData,
                                       protect,
                                       &oldProtect);
            }
        }

        // Call DllMain
        if (pNtHeaders->OptionalHeader.AddressOfEntryPoint) {
            auto DllMain = (BOOL(WINAPI*)(HINSTANCE, DWORD, LPVOID))((LPBYTE)pData->imageBase + pNtHeaders->OptionalHeader.AddressOfEntryPoint);
            DllMain((HINSTANCE)pData->imageBase, DLL_PROCESS_ATTACH, NULL);
        }

        return 0;
    }

    // Dummy function to mark end of shellcode
    DWORD WINAPI LoaderShellcodeEnd() {
        return 0;
    }

    bool ManualMapInject(HANDLE hProcess, const std::string& dllPath) {
        std::cout << "[*] Using manual mapping injection (stealth mode)\n";

        // Read DLL file
        std::ifstream file(dllPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "[!] Failed to open DLL file\n";
            return false;
        }

        size_t fileSize = file.tellg();
        std::vector<BYTE> fileData(fileSize);
        file.seekg(0, std::ios::beg);
        file.read((char*)fileData.data(), fileSize);
        file.close();

        // Parse PE headers
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)fileData.data();
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            std::cerr << "[!] Invalid DOS signature\n";
            return false;
        }

        PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)pDosHeader + pDosHeader->e_lfanew);
        if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
            std::cerr << "[!] Invalid NT signature\n";
            return false;
        }

        std::cout << "[+] DLL loaded into memory: " << fileSize << " bytes\n";
        std::cout << "[+] Image size: " << pNtHeaders->OptionalHeader.SizeOfImage << " bytes\n";

        // Allocate memory in target process
        LPVOID pRemoteImage = VirtualAllocEx(hProcess,
                                            NULL,
                                            pNtHeaders->OptionalHeader.SizeOfImage,
                                            MEM_COMMIT | MEM_RESERVE,
                                            PAGE_EXECUTE_READWRITE);

        if (!pRemoteImage) {
            std::cerr << "[!] Failed to allocate memory in target process\n";
            return false;
        }

        std::cout << "[+] Allocated memory at: 0x" << std::hex << (DWORD_PTR)pRemoteImage << std::dec << "\n";

        // Write headers
        if (!WriteProcessMemory(hProcess, pRemoteImage, fileData.data(), pNtHeaders->OptionalHeader.SizeOfHeaders, NULL)) {
            std::cerr << "[!] Failed to write headers\n";
            VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
            return false;
        }

        // Write sections
        PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
        for (WORD i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSectionHeader++) {
            if (pSectionHeader->SizeOfRawData) {
                if (!WriteProcessMemory(hProcess,
                                       (LPBYTE)pRemoteImage + pSectionHeader->VirtualAddress,
                                       fileData.data() + pSectionHeader->PointerToRawData,
                                       pSectionHeader->SizeOfRawData,
                                       NULL)) {
                    std::cerr << "[!] Failed to write section: " << pSectionHeader->Name << "\n";
                }
            }
        }

        std::cout << "[+] Wrote all sections to target process\n";

        // Prepare loader data
        LoaderData loaderData = {0};
        loaderData.imageBase = pRemoteImage;
        loaderData.ntHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)pRemoteImage + pDosHeader->e_lfanew);

        // Get relocation directory
        if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size) {
            loaderData.baseReloc = (PIMAGE_BASE_RELOCATION)((LPBYTE)pRemoteImage +
                pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        }

        // Get import directory
        if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
            loaderData.importDesc = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)pRemoteImage +
                pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        }

        // Get function addresses from kernel32
        HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
        loaderData.fnLoadLibraryA = (decltype(&LoadLibraryA))GetProcAddress(hKernel32, "LoadLibraryA");
        loaderData.fnGetProcAddress = (decltype(&GetProcAddress))GetProcAddress(hKernel32, "GetProcAddress");
        loaderData.fnVirtualProtect = (decltype(&VirtualProtect))GetProcAddress(hKernel32, "VirtualProtect");

        // Allocate memory for loader data
        LPVOID pLoaderData = VirtualAllocEx(hProcess, NULL, sizeof(LoaderData), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!pLoaderData) {
            std::cerr << "[!] Failed to allocate loader data\n";
            VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
            return false;
        }

        // Write loader data
        WriteProcessMemory(hProcess, pLoaderData, &loaderData, sizeof(LoaderData), NULL);

        // Allocate memory for shellcode
        DWORD shellcodeSize = (DWORD)((LPBYTE)LoaderShellcodeEnd - (LPBYTE)LoaderShellcode);
        LPVOID pShellcode = VirtualAllocEx(hProcess, NULL, shellcodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READ);
        if (!pShellcode) {
            std::cerr << "[!] Failed to allocate shellcode\n";
            VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
            VirtualFreeEx(hProcess, pLoaderData, 0, MEM_RELEASE);
            return false;
        }

        // Write shellcode
        WriteProcessMemory(hProcess, pShellcode, (LPVOID)LoaderShellcode, shellcodeSize, NULL);

        std::cout << "[+] Shellcode written, executing loader...\n";

        // Execute shellcode
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pShellcode, pLoaderData, 0, NULL);
        if (!hThread) {
            std::cerr << "[!] Failed to create remote thread\n";
            VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
            VirtualFreeEx(hProcess, pLoaderData, 0, MEM_RELEASE);
            VirtualFreeEx(hProcess, pShellcode, 0, MEM_RELEASE);
            return false;
        }

        // Wait for completion
        WaitForSingleObject(hThread, INFINITE);

        DWORD exitCode;
        GetExitCodeThread(hThread, &exitCode);
        CloseHandle(hThread);

        if (exitCode == 0) {
            std::cout << "[+] Manual mapping successful!\n";

            // Clean traces (optional - makes it stealthier)
            Sleep(100); // Give DLL time to initialize
            VirtualFreeEx(hProcess, pLoaderData, 0, MEM_RELEASE);
            VirtualFreeEx(hProcess, pShellcode, 0, MEM_RELEASE);

            std::cout << "[+] Cleaned injection traces\n";
            return true;
        } else {
            std::cerr << "[!] Shellcode returned error code: " << exitCode << "\n";
            VirtualFreeEx(hProcess, pRemoteImage, 0, MEM_RELEASE);
            VirtualFreeEx(hProcess, pLoaderData, 0, MEM_RELEASE);
            VirtualFreeEx(hProcess, pShellcode, 0, MEM_RELEASE);
            return false;
        }
    }

    bool LoadLibraryInject(HANDLE hProcess, const std::string& dllPath) {
        std::cout << "[*] Using LoadLibrary injection (standard mode)\n";

        // Convert to absolute path
        char fullPath[MAX_PATH];
        GetFullPathNameA(dllPath.c_str(), MAX_PATH, fullPath, NULL);

        // Allocate memory for DLL path
        LPVOID pRemotePath = VirtualAllocEx(hProcess, NULL, strlen(fullPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!pRemotePath) {
            std::cerr << "[!] Failed to allocate memory\n";
            return false;
        }

        // Write DLL path
        if (!WriteProcessMemory(hProcess, pRemotePath, fullPath, strlen(fullPath) + 1, NULL)) {
            std::cerr << "[!] Failed to write DLL path\n";
            VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
            return false;
        }

        // Get LoadLibraryA address
        LPVOID pLoadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
        if (!pLoadLibrary) {
            std::cerr << "[!] Failed to get LoadLibraryA address\n";
            VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
            return false;
        }

        // Create remote thread
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pRemotePath, 0, NULL);
        if (!hThread) {
            std::cerr << "[!] Failed to create remote thread\n";
            VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);
            return false;
        }

        // Wait for completion
        WaitForSingleObject(hThread, INFINITE);

        DWORD exitCode;
        GetExitCodeThread(hThread, &exitCode);
        CloseHandle(hThread);

        // Clean up
        VirtualFreeEx(hProcess, pRemotePath, 0, MEM_RELEASE);

        if (exitCode) {
            std::cout << "[+] LoadLibrary injection successful!\n";
            return true;
        } else {
            std::cerr << "[!] LoadLibrary failed in target process\n";
            return false;
        }
    }

    DWORD FindProcessId(const std::string& processName) {
        DWORD processId = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 processEntry;
            processEntry.dwSize = sizeof(PROCESSENTRY32);

            if (Process32First(snapshot, &processEntry)) {
                do {
                    if (_stricmp(processEntry.szExeFile, processName.c_str()) == 0) {
                        processId = processEntry.th32ProcessID;
                        break;
                    }
                } while (Process32Next(snapshot, &processEntry));
            }

            CloseHandle(snapshot);
        }

        return processId;
    }

    bool IsElevated() {
        BOOL elevated = FALSE;
        HANDLE token = NULL;

        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            TOKEN_ELEVATION elevation;
            DWORD size;

            if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
                elevated = elevation.TokenIsElevated;
            }

            CloseHandle(token);
        }

        return elevated != FALSE;
    }

    void CleanInjectionTraces(HANDLE hProcess, LPVOID allocatedMemory) {
        if (allocatedMemory) {
            VirtualFreeEx(hProcess, allocatedMemory, 0, MEM_RELEASE);
        }
    }
}
