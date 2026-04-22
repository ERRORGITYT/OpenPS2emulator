cmake_minimum_required(VERSION 3.10)
project(OpenPS2emulator LANGUAGES CXX)

# 1. Specify C++ standard per-target (better practice)
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/display.cpp
    ${PROJECT_SOURCE_DIR}/src/core/memory.cpp
    ${PROJECT_SOURCE_DIR}/src/core/cpu.cpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# 2. Find SDL2 and OpenGL (and prefer imported targets if available)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# 3. Modern include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/core
        $<TARGET_PROPERTY:SDL2::SDL2,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:OpenGL::GL,INTERFACE_INCLUDE_DIRECTORIES>
)

# 4. Link libraries properly using imported targets where possible
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL2::SDL2
        OpenGL::GL
)

# 5. Platform-specific libraries (example for Android/Termux OpenGL ES)
if(ANDROID OR "$ENV{PREFIX}" MATCHES "com.termux")
    target_link_libraries(${PROJECT_NAME} PRIVATE GLESv2)
endif()

# 6. Helpful status messages
message(STATUS "SDL2 Include Dirs: ${SDL2_INCLUDE_DIRS}")
message(STATUS "OpenGL Include Dirs: ${OPENGL_INCLUDE_DIR}")
