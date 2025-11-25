#include "../include/ESP.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <Windows.h>

ESP::ESP(MemoryReader& reader)
    : memoryReader(reader), gameBase(reader.GetBaseAddress()) {
}

ESP::~ESP() {
}

void ESP::Update() {
    DisplayESP();
}

Cube2::Player ESP::ReadLocalPlayer() {
    Cube2::Player localPlayer = {};

    uintptr_t localPlayerPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::LOCAL_PLAYER_OFFSET);
    if (localPlayerPtr != 0) {
        memoryReader.Read(localPlayerPtr, localPlayer);
    }

    return localPlayer;
}

std::vector<ESP::PlayerInfo> ESP::ReadAllPlayers() {
    std::vector<PlayerInfo> players;

    // Read local player for distance calculation
    Cube2::Player localPlayer = ReadLocalPlayer();

    // Read player count
    int32_t playerCount = memoryReader.Read<int32_t>(gameBase + Cube2::PLAYER_COUNT_OFFSET);

    // Sanity check
    if (playerCount <= 0 || playerCount > 32) {
        return players;
    }

    // Read entity list pointer
    uintptr_t entityListPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::ENTITY_LIST_OFFSET);
    if (entityListPtr == 0) {
        return players;
    }

    // Read all players
    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerPtr = memoryReader.Read<uintptr_t>(entityListPtr + (i * sizeof(uintptr_t)));

        if (playerPtr == 0) {
            continue;
        }

        Cube2::Player player = {};
        if (!memoryReader.Read(playerPtr, player)) {
            continue;
        }

        // Create player info
        PlayerInfo info;
        info.name = std::string(player.name);
        info.position = player.position;
        info.headPosition = player.headPosition;
        info.health = player.health;
        info.armor = player.armor;
        info.team = player.team;
        info.distance = localPlayer.position.Distance(player.position);
        info.isValid = (info.health > 0 && info.health <= 100);

        players.push_back(info);
    }

    return players;
}

void ESP::DisplayESP() {
    // Clear console
    system("cls");

    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         CUBE 2: SAUERBRATEN ESP - ANTICHEAT TESTING TOOL               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    // Read local player
    Cube2::Player localPlayer = ReadLocalPlayer();

    std::cout << "┌─ LOCAL PLAYER ─────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Name:     " << std::setw(15) << std::left << localPlayer.name << std::endl;
    std::cout << "│ Health:   " << GetHealthBar(localPlayer.health) << " [" << localPlayer.health << "/100]" << std::endl;
    std::cout << "│ Armor:    " << GetHealthBar(localPlayer.armor) << " [" << localPlayer.armor << "/100]" << std::endl;
    std::cout << "│ Position: X:" << std::fixed << std::setprecision(1)
              << localPlayer.position.x << " Y:" << localPlayer.position.y
              << " Z:" << localPlayer.position.z << std::endl;
    std::cout << "│ Weapon:   ID " << localPlayer.currentWeapon << std::endl;
    std::cout << "└────────────────────────────────────────────────────────────────────────┘" << std::endl;
    std::cout << std::endl;

    // Read all players
    std::vector<PlayerInfo> players = ReadAllPlayers();

    std::cout << "┌─ ENEMY PLAYERS [" << players.size() << "] ──────────────────────────────────────────────────┐" << std::endl;

    if (players.empty()) {
        std::cout << "│ No players detected in game                                           │" << std::endl;
    } else {
        for (size_t i = 0; i < players.size(); i++) {
            PrintPlayerInfo(players[i], i + 1);
        }
    }

    std::cout << "└────────────────────────────────────────────────────────────────────────┘" << std::endl;
    std::cout << std::endl;
    std::cout << "Press Ctrl+C to exit..." << std::endl;
}

void ESP::PrintPlayerInfo(const PlayerInfo& player, int index) {
    if (!player.isValid) {
        return;
    }

    std::cout << "│ [" << index << "] " << std::setw(12) << std::left << player.name
              << " │ HP: " << GetHealthBar(player.health, 100)
              << " [" << std::setw(3) << player.health << "]"
              << " │ AR: " << std::setw(3) << player.armor
              << " │ Dist: " << std::fixed << std::setprecision(1) << std::setw(6) << player.distance << "m"
              << " │" << std::endl;

    std::cout << "│     Pos: X:" << std::fixed << std::setprecision(1) << std::setw(7) << player.position.x
              << " Y:" << std::setw(7) << player.position.y
              << " Z:" << std::setw(7) << player.position.z
              << " Team: " << GetTeamName(player.team) << "     │" << std::endl;
}

std::string ESP::GetTeamName(int32_t team) {
    switch (team) {
        case 0: return "NONE";
        case 1: return "GOOD";
        case 2: return "EVIL";
        default: return "UNKNOWN";
    }
}

std::string ESP::GetHealthBar(int32_t health, int32_t maxHealth) {
    int barLength = 10;
    int filledLength = (health * barLength) / maxHealth;

    if (filledLength > barLength) filledLength = barLength;
    if (filledLength < 0) filledLength = 0;

    std::string bar = "[";
    for (int i = 0; i < barLength; i++) {
        if (i < filledLength) {
            bar += "█";
        } else {
            bar += "░";
        }
    }
    bar += "]";

    return bar;
}
