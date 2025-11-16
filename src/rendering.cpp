#include "rendering.h"
#include <cstring>

namespace Rendering {
    HDC g_hdc = nullptr;
    static GLuint fontBase = 0;
    static bool fontInitialized = false;

    void InitializeFont() {
        if (fontInitialized || !g_hdc) return;

        // Create font display lists
        fontBase = glGenLists(256);
        if (fontBase == 0) return;

        // Create a simple font
        HFONT hFont = CreateFontA(
            14,                        // Height
            0,                         // Width
            0,                         // Escapement
            0,                         // Orientation
            FW_NORMAL,                 // Weight
            FALSE,                     // Italic
            FALSE,                     // Underline
            FALSE,                     // StrikeOut
            ANSI_CHARSET,              // CharSet
            OUT_TT_PRECIS,             // OutputPrecision
            CLIP_DEFAULT_PRECIS,       // ClipPrecision
            ANTIALIASED_QUALITY,       // Quality
            FF_DONTCARE | DEFAULT_PITCH, // PitchAndFamily
            "Arial"                    // Face name
        );

        if (!hFont) {
            glDeleteLists(fontBase, 256);
            return;
        }

        HFONT hOldFont = (HFONT)SelectObject(g_hdc, hFont);

        // Build the bitmap font
        if (wglUseFontBitmapsA(g_hdc, 0, 256, fontBase)) {
            fontInitialized = true;
        } else {
            glDeleteLists(fontBase, 256);
            fontBase = 0;
        }

        SelectObject(g_hdc, hOldFont);
        DeleteObject(hFont);
    }

    void CleanupFont() {
        if (fontBase != 0) {
            glDeleteLists(fontBase, 256);
            fontBase = 0;
        }
        fontInitialized = false;
    }

    void DrawText(float x, float y, const char* text, float r, float g, float b) {
        if (!text || strlen(text) == 0) return;

        // Initialize font if needed
        if (!fontInitialized && g_hdc) {
            InitializeFont();
        }

        if (!fontInitialized) return;

        // Set text color
        glColor3f(r, g, b);

        // Set raster position
        glRasterPos2f(x, y);

        // Save current list base
        glPushAttrib(GL_LIST_BIT);

        // Set font list base and draw text
        glListBase(fontBase);
        glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

        // Restore list base
        glPopAttrib();
    }
}
