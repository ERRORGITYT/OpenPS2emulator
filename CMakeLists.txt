cmake_minimum_required(VERSION 3.10)
project(OpenPS2emulator LANGUAGES CXX)

# Enforce C++17 standard (or whatever is appropriate)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2, prefer config mode for cleaner targets
find_package(SDL2 REQUIRED CONFIG)

# Collect all source files
set(SOURCES
    src/main.cpp
    src/core/memory.cpp
    src/core/cpu.cpp
    src/display.cpp
)

# Main executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories per-target
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/core
    $<TARGET_PROPERTY:SDL2::SDL2,INTERFACE_INCLUDE_DIRECTORIES>
)

# Prefer target style linking if available
if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

# Set compile features (optional, but recommended)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Installation (optional)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
