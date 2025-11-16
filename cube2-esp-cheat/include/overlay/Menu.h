#pragma once
#include "../Config.h"
#include "../MemoryReader.h"

class Menu {
private:
    bool initialized;
    float menuWidth;
    float menuHeight;

    // Tab system
    int currentTab;
    enum Tabs {
        TAB_AIMBOT = 0,
        TAB_ESP,
        TAB_WALLHACK,
        TAB_MISC,
        TAB_SETTINGS,
        TAB_COUNT
    };

public:
    Menu();
    ~Menu();

    void Render(MemoryReader& memReader);

private:
    void RenderHeader();
    void RenderTabs();
    void RenderTabContent();

    // Tab content renderers
    void RenderAimbotTab();
    void RenderESPTab();
    void RenderWallhackTab();
    void RenderMiscTab();
    void RenderSettingsTab();

    // Helper functions
    void RenderColorPicker(const char* label, float color[3]);
    void RenderHotkeySelector(const char* label, int& key);
    const char* GetKeyName(int key);
};
