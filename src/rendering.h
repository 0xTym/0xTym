#pragma once
#include <Windows.h>
#include <gl/GL.h>

namespace Rendering {
    // Current HDC from the hooked wglSwapBuffers
    extern HDC g_hdc;

    // Initialize font for text rendering
    void InitializeFont();

    // Cleanup font resources
    void CleanupFont();

    // Draw text using Windows GDI bitmap fonts
    void DrawText(float x, float y, const char* text, float r, float g, float b);
}
