#include "../include/MemoryReader.h"
#include "../include/ESP.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>

// Configuration
constexpr int UPDATE_INTERVAL_MS = 100; // Update ESP every 100ms
const std::wstring GAME_PROCESS_NAME = L"sauerbraten.exe";

bool isRunning = true;

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        std::cout << "\nShutting down ESP..." << std::endl;
        isRunning = false;
        return TRUE;
    }
    return FALSE;
}

void PrintBanner() {
    std::cout << R"(
  _____ _    _ ____  ______   ___    ______  _____ _____
 / ____| |  | |  _ \|  ____| |__ \  |  ____|/ ____|  __ \
| |    | |  | | |_) | |__       ) | | |__  | (___ | |__) |
| |    | |  | |  _ <|  __|     / /  |  __|  \___ \|  ___/
| |____| |__| | |_) | |____   / /_  | |____ ____) | |
 \_____|\____/|____/|______| |____| |______|_____/|_|

        Sauerbraten ESP - Anticheat Testing Tool
                    By 0xTym
    )" << std::endl;
    std::cout << std::endl;
}

int main() {
    // Set console title
    SetConsoleTitleW(L"Cube 2: Sauerbraten ESP - Anticheat Testing");

    // Set console handler for graceful shutdown
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
        std::cerr << "Could not set control handler" << std::endl;
        return 1;
    }

    PrintBanner();

    // Create memory reader
    MemoryReader memReader;

    std::wcout << L"Attempting to attach to " << GAME_PROCESS_NAME << L"..." << std::endl;
    std::wcout << L"Make sure the game is running!" << std::endl;
    std::wcout << std::endl;

    // Try to attach to the game process
    while (!memReader.AttachToProcess(GAME_PROCESS_NAME) && isRunning) {
        std::wcout << L"Waiting for game process..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    if (!memReader.IsAttached()) {
        std::cerr << "Failed to attach to game process." << std::endl;
        return 1;
    }

    std::cout << std::endl;
    std::cout << "Successfully attached! Starting ESP..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Create ESP
    ESP esp(memReader);

    // Main ESP loop
    while (isRunning) {
        try {
            esp.Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_INTERVAL_MS));
        }
        catch (const std::exception& e) {
            std::cerr << "Error in ESP loop: " << e.what() << std::endl;
            isRunning = false;
        }
    }

    std::cout << "ESP stopped. Goodbye!" << std::endl;
    return 0;
}
