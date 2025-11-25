#include "../../include/overlay/Menu.h"
#include "../../external/imgui/imgui.h"
#include <Windows.h>

Menu::Menu()
    : initialized(false), menuWidth(700), menuHeight(500), currentTab(TAB_AIMBOT) {
}

Menu::~Menu() {
}

void Menu::Render(MemoryReader* memReader) {
    using namespace CheatConfig;

    if (!g_Config.menu.visible) {
        return;
    }

    // Note: memReader is not used in menu rendering (only for external version status)
    // For internal version, memReader will be nullptr

    // Set window style
    ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(g_Config.menu.opacity);

    // Main window
    if (ImGui::Begin("Cube 2: Sauerbraten - Advanced Cheat Suite", &g_Config.menu.visible,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {

        RenderHeader();
        RenderTabs();

        ImGui::Separator();

        RenderTabContent();

        ImGui::End();
    }
}

void Menu::RenderHeader() {
    using namespace CheatConfig;

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.5f, 1.0f));
    ImGui::Text("CUBE 2 CHEAT v1.0 - by 0xTym");
    ImGui::PopStyleColor();

    ImGui::SameLine(menuWidth - 200);

    if (g_Config.menu.showFPS) {
        ImGui::Text("FPS: %d | %.1f ms", g_Config.fps, g_Config.deltaTime * 1000.0f);
    }

    ImGui::Separator();
}

void Menu::RenderTabs() {
    const char* tabNames[] = {
        "Aimbot",
        "ESP",
        "Wallhack",
        "Misc",
        "Settings"
    };

    ImGui::BeginChild("Tabs", ImVec2(120, 0), true);

    for (int i = 0; i < TAB_COUNT; i++) {
        if (currentTab == i) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        }

        if (ImGui::Button(tabNames[i], ImVec2(110, 40))) {
            currentTab = i;
        }

        if (currentTab == i) {
            ImGui::PopStyleColor();
        }
    }

    ImGui::EndChild();
    ImGui::SameLine();
}

void Menu::RenderTabContent() {
    ImGui::BeginChild("Content", ImVec2(0, 0), true);

    switch (currentTab) {
        case TAB_AIMBOT:
            RenderAimbotTab();
            break;
        case TAB_ESP:
            RenderESPTab();
            break;
        case TAB_WALLHACK:
            RenderWallhackTab();
            break;
        case TAB_MISC:
            RenderMiscTab();
            break;
        case TAB_SETTINGS:
            RenderSettingsTab();
            break;
    }

    ImGui::EndChild();
}

void Menu::RenderAimbotTab() {
    using namespace CheatConfig;

    ImGui::Text("Aimbot Configuration");
    ImGui::Separator();

    ImGui::Checkbox("Enable Aimbot", &g_Config.aimbot.enabled);

    if (g_Config.aimbot.enabled) {
        ImGui::Spacing();

        ImGui::Checkbox("Smoothing", &g_Config.aimbot.smoothing);

        if (g_Config.aimbot.smoothing) {
            ImGui::SliderFloat("Smoothness", &g_Config.aimbot.smoothness, 1.0f, 20.0f, "%.1f");
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Higher = Smoother (less obvious)");
        }

        ImGui::Spacing();
        ImGui::SliderFloat("FOV", &g_Config.aimbot.fov, 1.0f, 180.0f, "%.0f degrees");
        ImGui::SliderFloat("Max Distance", &g_Config.aimbot.maxDistance, 50.0f, 1000.0f, "%.0f");

        ImGui::Spacing();
        ImGui::Checkbox("Target Head", &g_Config.aimbot.targetHead);
        ImGui::SameLine();
        ImGui::Checkbox("Target Chest", &g_Config.aimbot.targetChest);

        ImGui::Spacing();
        ImGui::Checkbox("Visible Only", &g_Config.aimbot.visibleOnly);
        ImGui::Checkbox("Team Check", &g_Config.aimbot.teamCheck);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Don't aim at teammates");

        ImGui::Spacing();
        ImGui::Text("Aim Key:");
        ImGui::SameLine();
        RenderHotkeySelector("##AimKey", g_Config.aimbot.aimKey);
    }
}

void Menu::RenderESPTab() {
    using namespace CheatConfig;

    ImGui::Text("ESP Configuration");
    ImGui::Separator();

    ImGui::Checkbox("Enable ESP", &g_Config.esp.enabled);

    if (g_Config.esp.enabled) {
        ImGui::Spacing();

        ImGui::Checkbox("Show Boxes", &g_Config.esp.showBoxes);
        ImGui::Checkbox("Show Names", &g_Config.esp.showNames);
        ImGui::Checkbox("Show Health", &g_Config.esp.showHealth);
        ImGui::Checkbox("Show Distance", &g_Config.esp.showDistance);
        ImGui::Checkbox("Show Skeleton", &g_Config.esp.showSkeleton);

        ImGui::Spacing();
        ImGui::SliderFloat("Max Distance", &g_Config.esp.maxDistance, 50.0f, 1000.0f, "%.0f");
        ImGui::SliderFloat("Box Thickness", &g_Config.esp.boxThickness, 1.0f, 5.0f, "%.1f");

        ImGui::Spacing();
        ImGui::Text("Colors:");
        RenderColorPicker("Enemy Color", g_Config.esp.enemyColor);
        RenderColorPicker("Team Color", g_Config.esp.teamColor);
    }
}

void Menu::RenderWallhackTab() {
    using namespace CheatConfig;

    ImGui::Text("Wallhack Configuration");
    ImGui::Separator();

    ImGui::Checkbox("Enable Wallhack", &g_Config.wallhack.enabled);

    if (g_Config.wallhack.enabled) {
        ImGui::Spacing();

        ImGui::Checkbox("Glow Effect", &g_Config.wallhack.glowEnabled);
        ImGui::Checkbox("Chams (Colored Models)", &g_Config.wallhack.chamEnabled);

        if (g_Config.wallhack.glowEnabled) {
            ImGui::Spacing();
            ImGui::SliderFloat("Glow Brightness", &g_Config.wallhack.glowBrightness, 0.5f, 3.0f, "%.1f");

            RenderColorPicker("Enemy Glow", g_Config.wallhack.enemyGlow);
            RenderColorPicker("Team Glow", g_Config.wallhack.teamGlow);
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Note: Some features require game-specific offsets");
    }
}

void Menu::RenderMiscTab() {
    using namespace CheatConfig;

    ImGui::Text("Miscellaneous Features");
    ImGui::Separator();

    ImGui::Checkbox("No Recoil", &g_Config.misc.noRecoil);
    ImGui::Checkbox("No Spread", &g_Config.misc.noSpread);
    ImGui::Checkbox("Infinite Ammo", &g_Config.misc.infiniteAmmo);
    ImGui::Checkbox("Rapid Fire", &g_Config.misc.rapidFire);

    ImGui::Spacing();
    ImGui::Checkbox("Speed Hack", &g_Config.misc.speedHack);

    if (g_Config.misc.speedHack) {
        ImGui::SliderFloat("Speed Multiplier", &g_Config.misc.speedMultiplier, 1.0f, 5.0f, "%.1fx");
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: Some features may be detectable!");
}

void Menu::RenderSettingsTab() {
    using namespace CheatConfig;

    ImGui::Text("Menu Settings");
    ImGui::Separator();

    ImGui::Checkbox("Show FPS", &g_Config.menu.showFPS);
    ImGui::SliderFloat("Menu Opacity", &g_Config.menu.opacity, 0.3f, 1.0f, "%.2f");

    ImGui::Spacing();
    ImGui::Text("Menu Toggle Key:");
    ImGui::SameLine();
    RenderHotkeySelector("##MenuKey", g_Config.menu.menuKey);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("About");
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Cube 2: Sauerbraten Cheat Suite");
    ImGui::Text("Version: 1.0.0");
    ImGui::Text("Author: 0xTym");
    ImGui::Text("Purpose: Anticheat Testing & Security Research");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "FOR EDUCATIONAL PURPOSES ONLY");
}

void Menu::RenderColorPicker(const char* label, float color[3]) {
    ImGui::ColorEdit3(label, color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();
    ImGui::Text("%s", label);
}

void Menu::RenderHotkeySelector(const char* label, int& key) {
    static bool listening = false;
    static int* listenTarget = nullptr;

    if (listenTarget == &key && listening) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
        if (ImGui::Button("Press Any Key...", ImVec2(150, 0))) {
            listening = false;
            listenTarget = nullptr;
        }
        ImGui::PopStyleColor();

        // Check for key press
        for (int vk = 0; vk < 256; vk++) {
            if (GetAsyncKeyState(vk) & 0x8000) {
                if (vk != VK_ESCAPE) {
                    key = vk;
                }
                listening = false;
                listenTarget = nullptr;
                break;
            }
        }
    } else {
        if (ImGui::Button(GetKeyName(key), ImVec2(150, 0))) {
            listening = true;
            listenTarget = &key;
        }
    }
}

const char* Menu::GetKeyName(int key) {
    static char keyName[32];

    switch (key) {
        case VK_LBUTTON: return "Left Mouse";
        case VK_RBUTTON: return "Right Mouse";
        case VK_MBUTTON: return "Middle Mouse";
        case VK_XBUTTON1: return "Mouse 4";
        case VK_XBUTTON2: return "Mouse 5";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case VK_HOME: return "Home";
        case VK_END: return "End";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        default:
            if (key >= 0x30 && key <= 0x5A) { // 0-9, A-Z
                sprintf_s(keyName, "%c", (char)key);
                return keyName;
            }
            sprintf_s(keyName, "Key %d", key);
            return keyName;
    }
}
