cmake_minimum_required(VERSION 3.10)
project(OpenPS2emulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find SDL2
find_package(SDL2 REQUIRED)

# 2. Find OpenGL (Crucial for fixing your linker errors)
# On Android/Termux, this usually links to GLESv2
find_package(OpenGL REQUIRED)

# 3. Include directories for your project and libraries
include_directories(
    src 
    src/core 
    ${SDL2_INCLUDE_DIRS} 
    ${OPENGL_INCLUDE_DIR}
)

# 4. List all your source files
# Make sure these filenames match exactly what is in your folders!
set(SOURCES
    src/main.cpp 
    src/display.cpp
    src/core/memory.cpp 
    src/core/cpu.cpp
)

# 5. Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# 6. Link the libraries
# This tells the linker WHERE to find those "undefined symbols"
target_link_libraries(${PROJECT_NAME} 
    ${SDL2_LIBRARIES} 
    ${OPENGL_LIBRARIES}
    -lGLESv2  # Specific link for Android/Termux environments
)
