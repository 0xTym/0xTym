#pragma once
#include "MemoryReader.h"
#include "GameStructures.h"
#include "Config.h"

class Wallhack {
private:
    MemoryReader& memoryReader;
    uintptr_t gameBase;

    // Common wallhack technique: modify render distance or visibility flags
    struct RenderSettings {
        float originalRenderDistance;
        bool modified;
    };

    RenderSettings renderSettings;

public:
    Wallhack(MemoryReader& reader);
    ~Wallhack();

    void Enable();
    void Disable();
    void Update();

private:
    void ApplyGlowEffect();
    void ApplyChamEffect();
    void ModifyVisibilityFlags();
    void RestoreOriginalSettings();
};
