cmake_minimum_required(VERSION 3.15)
project(Cube2ESP VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ImGui path
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

# Check if ImGui exists
if(NOT EXISTS ${IMGUI_DIR}/imgui.cpp)
    message(FATAL_ERROR
        "\n"
        "========================================\n"
        " ImGui NOT FOUND!\n"
        "========================================\n"
        "\n"
        "ImGui is required to build this project.\n"
        "\n"
        "QUICK FIX (Windows):\n"
        "  1. Open PowerShell in the 'external' folder\n"
        "  2. Run: .\\download_imgui.ps1\n"
        "     Or: .\\download_imgui.bat\n"
        "\n"
        "MANUAL INSTALLATION:\n"
        "  See external/README.md or external/IMGUI_SETUP.md\n"
        "\n"
        "After installing ImGui, delete the build folder and run CMake again.\n"
        "========================================\n"
    )
endif()

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/imgui_impl_win32.cpp
)

# Project source files
set(SOURCES
    src/main.cpp
    src/MemoryReader.cpp
    src/ESP.cpp
    src/Aimbot.cpp
    src/Wallhack.cpp
    src/overlay/Menu.cpp
)

# Header files
set(HEADERS
    include/MemoryReader.h
    include/ESP.h
    include/GameStructures.h
    include/Config.h
    include/Aimbot.h
    include/Wallhack.h
    include/overlay/Menu.h
    include/overlay/Overlay.h
)

# Create executable
add_executable(Cube2ESP ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})

# Include directories
target_include_directories(Cube2ESP PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/overlay
    ${IMGUI_DIR}
)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(Cube2ESP PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )

    # Link DirectX11 and other Windows libraries
    target_link_libraries(Cube2ESP PRIVATE
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
        dwmapi.lib
    )

    # Set subsystem to windows (not console) for overlay
    set_target_properties(Cube2ESP PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

    # Enable static linking for better portability
    if(MSVC)
        set_property(TARGET Cube2ESP PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        # Warning level
        target_compile_options(Cube2ESP PRIVATE /W4 /wd4100 /wd4201)

        # Disable specific warnings for ImGui
        target_compile_options(Cube2ESP PRIVATE /wd4996)
    else()
        # MinGW or other compilers
        target_link_options(Cube2ESP PRIVATE -static -mwindows)
        target_compile_options(Cube2ESP PRIVATE -Wall -Wextra -Wno-unused-parameter)
    endif()
else()
    message(FATAL_ERROR "This project is designed for Windows only")
endif()

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "ImGui directory: ${IMGUI_DIR}")
