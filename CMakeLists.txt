cmake_minimum_required(VERSION 3.10)
project(SauerbratenESP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build loader as subdirectory
add_subdirectory(loader)

# Set output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SOURCES
    src/main.cpp
    src/esp.cpp
    src/hooks.cpp
    src/rendering.cpp
)

# Create shared library (DLL)
add_library(SauerbratenESP SHARED ${SOURCES})

# Link libraries
if(WIN32)
    target_link_libraries(SauerbratenESP PRIVATE
        opengl32
        glu32
        gdi32
    )
else()
    target_link_libraries(SauerbratenESP PRIVATE
        GL
        GLU
    )
endif()

# Set compile options
if(MSVC)
    target_compile_options(SauerbratenESP PRIVATE /W4)
    # Disable warnings about unsafe functions
    target_compile_definitions(SauerbratenESP PRIVATE _CRT_SECURE_NO_WARNINGS)
    # Set subsystem to windows and entry point for DLL
    set_target_properties(SauerbratenESP PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /DLL"
    )
else()
    target_compile_options(SauerbratenESP PRIVATE -Wall -Wextra -fpermissive)
    set_target_properties(SauerbratenESP PROPERTIES
        LINK_FLAGS "-shared"
    )
endif()
