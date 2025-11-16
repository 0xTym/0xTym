#include "hooks.h"
#include "esp.h"
#include <cstring>

namespace Hooks {
    twglSwapBuffers oWglSwapBuffers = nullptr;

    // Simple inline hook implementation
    bool HookFunction(void* targetFunc, void* hookFunc, void** originalFunc) {
        DWORD oldProtect;

        // Change memory protection
        if (!VirtualProtect(targetFunc, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            return false;
        }

        // Allocate trampoline
        void* trampoline = VirtualAlloc(nullptr, 20, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!trampoline) {
            VirtualProtect(targetFunc, 5, oldProtect, &oldProtect);
            return false;
        }

        // Copy original bytes to trampoline
        memcpy(trampoline, targetFunc, 5);

        // Add jump back to original function + 5
        unsigned char* trampolineBytes = static_cast<unsigned char*>(trampoline);
        trampolineBytes[5] = 0xE9; // JMP
        *reinterpret_cast<DWORD*>(&trampolineBytes[6]) =
            reinterpret_cast<DWORD>(targetFunc) + 5 - (reinterpret_cast<DWORD>(trampoline) + 10);

        // Write jump to hook function
        unsigned char* targetBytes = static_cast<unsigned char*>(targetFunc);
        targetBytes[0] = 0xE9; // JMP
        *reinterpret_cast<DWORD*>(&targetBytes[1]) =
            reinterpret_cast<DWORD>(hookFunc) - reinterpret_cast<DWORD>(targetFunc) - 5;

        // Restore protection
        VirtualProtect(targetFunc, 5, oldProtect, &oldProtect);

        *originalFunc = trampoline;
        return true;
    }

    bool UnhookFunction(void* targetFunc, void* originalFunc) {
        DWORD oldProtect;

        if (!VirtualProtect(targetFunc, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            return false;
        }

        // Restore original bytes
        memcpy(targetFunc, originalFunc, 5);

        VirtualProtect(targetFunc, 5, oldProtect, &oldProtect);
        VirtualFree(originalFunc, 0, MEM_RELEASE);

        return true;
    }

    BOOL WINAPI hkWglSwapBuffers(HDC hdc) {
        // Render ESP
        if (ESP::config.enabled) {
            ESP::Render();
        }

        // Call original function
        return oWglSwapBuffers(hdc);
    }

    bool Initialize() {
        // Get wglSwapBuffers address
        HMODULE opengl32 = GetModuleHandleA("opengl32.dll");
        if (!opengl32) {
            return false;
        }

        void* wglSwapBuffersAddr = GetProcAddress(opengl32, "wglSwapBuffers");
        if (!wglSwapBuffersAddr) {
            return false;
        }

        // Hook wglSwapBuffers
        if (!HookFunction(wglSwapBuffersAddr, hkWglSwapBuffers,
                         reinterpret_cast<void**>(&oWglSwapBuffers))) {
            return false;
        }

        return true;
    }

    void Shutdown() {
        if (oWglSwapBuffers) {
            HMODULE opengl32 = GetModuleHandleA("opengl32.dll");
            if (opengl32) {
                void* wglSwapBuffersAddr = GetProcAddress(opengl32, "wglSwapBuffers");
                if (wglSwapBuffersAddr) {
                    UnhookFunction(wglSwapBuffersAddr, oWglSwapBuffers);
                }
            }
        }
    }
}
