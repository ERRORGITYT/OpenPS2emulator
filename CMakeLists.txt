cmake_minimum_required(VERSION 3.10)
project(OpenPS2emulator)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include directories
include_directories(src src/core ${SDL2_INCLUDE_DIRS})

# Add your source files
add_executable(chip8_test 
    src/main.cpp 
    src/core/memory.cpp 
    src/core/cpu.cpp 
    src/display.cpp
)

# Link SDL2 to your executable
target_link_libraries(chip8_test ${SDL2_LIBRARIES})
