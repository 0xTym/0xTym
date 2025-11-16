#pragma once
#include <Windows.h>
#include <gl/GL.h>

namespace Hooks {
    // Function pointer types
    typedef BOOL(WINAPI* twglSwapBuffers)(HDC hdc);

    // Original function pointers
    extern twglSwapBuffers oWglSwapBuffers;

    // Hook functions
    BOOL WINAPI hkWglSwapBuffers(HDC hdc);

    // Initialize and remove hooks
    bool Initialize();
    void Shutdown();

    // Trampoline hook helper
    bool HookFunction(void* targetFunc, void* hookFunc, void** originalFunc);
    bool UnhookFunction(void* targetFunc, void* originalFunc);
}
