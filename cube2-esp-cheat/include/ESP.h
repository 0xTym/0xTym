#pragma once
#include "MemoryReader.h"
#include "GameStructures.h"
#include <vector>
#include <string>

class ESP {
private:
    MemoryReader& memoryReader;
    uintptr_t gameBase;

    struct PlayerInfo {
        std::string name;
        Cube2::Vector3 position;
        Cube2::Vector3 headPosition;
        int32_t health;
        int32_t armor;
        int32_t team;
        float distance;
        bool isValid;
    };

public:
    ESP(MemoryReader& reader);
    ~ESP();

    void Update();
    void DisplayESP();

private:
    Cube2::Player ReadLocalPlayer();
    std::vector<PlayerInfo> ReadAllPlayers();
    void PrintPlayerInfo(const PlayerInfo& player, int index);
    std::string GetTeamName(int32_t team);
    std::string GetHealthBar(int32_t health, int32_t maxHealth = 100);
};
