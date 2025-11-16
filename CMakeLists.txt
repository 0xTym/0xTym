cmake_minimum_required(VERSION 3.10)
project(SauerbratenESP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
target_link_libraries(SauerbratenESP
    opengl32
    glu32
)

# Set compile options
if(MSVC)
    target_compile_options(SauerbratenESP PRIVATE /W4)
else()
    target_compile_options(SauerbratenESP PRIVATE -Wall -Wextra -fpermissive)
endif()
