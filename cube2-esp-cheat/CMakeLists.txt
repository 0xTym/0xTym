cmake_minimum_required(VERSION 3.15)
project(Cube2ESP VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SOURCES
    src/main.cpp
    src/MemoryReader.cpp
    src/ESP.cpp
)

# Header files
set(HEADERS
    include/MemoryReader.h
    include/ESP.h
    include/GameStructures.h
)

# Create executable
add_executable(Cube2ESP ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(Cube2ESP PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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

    # Set subsystem to console
    set_target_properties(Cube2ESP PROPERTIES
        WIN32_EXECUTABLE FALSE
    )

    # Enable static linking for better portability
    if(MSVC)
        set_property(TARGET Cube2ESP PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        # Warning level
        target_compile_options(Cube2ESP PRIVATE /W4)
    else()
        # MinGW or other compilers
        target_link_options(Cube2ESP PRIVATE -static)
        target_compile_options(Cube2ESP PRIVATE -Wall -Wextra)
    endif()
else()
    message(WARNING "This project is designed for Windows only")
endif()

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
