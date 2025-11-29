cmake_minimum_required(VERSION 3.10)
project(SauerbratenLoader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Source files
set(SOURCES
    main.cpp
    loader.cpp
)

# Create executable
add_executable(SauerbratenLoader ${SOURCES})

# Set compile options
if(MSVC)
    target_compile_options(SauerbratenLoader PRIVATE /W4)
    target_compile_definitions(SauerbratenLoader PRIVATE _CRT_SECURE_NO_WARNINGS)

    # Request administrator privileges
    set_target_properties(SauerbratenLoader PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )
else()
    target_compile_options(SauerbratenLoader PRIVATE -Wall -Wextra)
endif()

# Installation (optional)
install(TARGETS SauerbratenLoader
    RUNTIME DESTINATION bin
)
