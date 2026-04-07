#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <chrono>
#include <memory>

// PS2 Constants
constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 448;
constexpr int TARGET_FPS = 60;
constexpr float FRAME_TIME_MS = 1000.0f / TARGET_FPS;

// Logging utility
void LogError(const char* context, const char* error) {
    std::cerr << "[ERROR] " << context << ": " << error << std::endl;
}

void LogInfo(const char* message) {
    std::cout << "[INFO] " << message << std::endl;
}

// Window and context structure for better resource management
struct GraphicsContext {
    SDL_Window* window;
    SDL_GLContext gl_context;

    GraphicsContext() : window(nullptr), gl_context(nullptr) {}

    ~GraphicsContext() {
        Cleanup();
    }

    bool Initialize() {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            LogError("SDL Initialization", SDL_GetError());
            return false;
        }
        LogInfo("SDL initialized successfully");

        // Configure OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

        // Create window
        window = SDL_CreateWindow(
            "OpenPS2Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );

        if (!window) {
            LogError("Window Creation", SDL_GetError());
            SDL_Quit();
            return false;
        }
        LogInfo("Window created successfully");

        // Create OpenGL context
        gl_context = SDL_GL_CreateContext(window);
        if (!gl_context) {
            LogError("OpenGL Context Creation", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        LogInfo("OpenGL context created successfully");

        // Enable VSync for smoother frame delivery
        SDL_GL_SetSwapInterval(1);

        // Configure OpenGL
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        LogInfo("Graphics context fully initialized");
        return true;
    }

    void Cleanup() {
        if (gl_context) {
            SDL_GL_DeleteContext(gl_context);
            gl_context = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
        LogInfo("Graphics context cleaned up");
    }

    bool IsValid() const {
        return window != nullptr && gl_context != nullptr;
    }
};

// Frame rate manager for accurate timing
class FrameRateManager {
private:
    std::chrono::high_resolution_clock::time_point frame_start;
    float delta_time;
    int frame_count;
    float accumulated_time;

public:
    FrameRateManager() : delta_time(0.0f), frame_count(0), accumulated_time(0.0f) {
        frame_start = std::chrono::high_resolution_clock::now();
    }

    void StartFrame() {
        frame_start = std::chrono::high_resolution_clock::now();
    }

    void EndFrame() {
        auto frame_end = std::chrono::high_resolution_clock::now();
        float elapsed_ms = std::chrono::duration<float, std::milli>(frame_end - frame_start).count();
        delta_time = elapsed_ms / 1000.0f;
        
        frame_count++;
        accumulated_time += elapsed_ms;

        // Maintain target FPS with adaptive sleep
        if (elapsed_ms < FRAME_TIME_MS) {
            SDL_Delay(static_cast<Uint32>(FRAME_TIME_MS - elapsed_ms));
        }

        // Log FPS every second
        if (accumulated_time >= 1000.0f) {
            LogInfo(("FPS: " + std::to_string(frame_count)).c_str());
            frame_count = 0;
            accumulated_time = 0.0f;
        }
    }

    float GetDeltaTime() const { return delta_time; }
};

// Event handler
class InputHandler {
public:
    InputHandler() : should_close(false) {}

    void ProcessEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    should_close = true;
                    break;
                case SDL_KEYDOWN:
                    HandleKeyDown(event.key);
                    break;
                case SDL_KEYUP:
                    HandleKeyUp(event.key);
                    break;
                case SDL_WINDOWEVENT:
                    HandleWindowEvent(event.window);
                    break;
                default:
                    break;
            }
        }
    }

    bool ShouldClose() const { return should_close; }

private:
    bool should_close;

    void HandleKeyDown(const SDL_KeyboardEvent& key) {
        if (key.keysym.sym == SDLK_ESCAPE) {
            should_close = true;
        }
        // Add more key handling as needed
    }

    void HandleKeyUp(const SDL_KeyboardEvent& key) {
        // Handle key releases
    }

    void HandleWindowEvent(const SDL_WindowEvent& window_event) {
        if (window_event.event == SDL_WINDOWEVENT_RESIZED) {
            glViewport(0, 0, window_event.data1, window_event.data2);
            LogInfo("Window resized");
        }
    }
};

// Renderer for graphics operations
class Renderer {
public:
    void Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Present(SDL_Window* window) {
        SDL_GL_SwapWindow(window);
    }

    void RenderFrame() {
        // Placeholder for actual rendering logic
        // This will be expanded when implementing PS2 emulation graphics
    }
};

// Main application class
class PS2Emulator {
private:
    GraphicsContext graphics;
    FrameRateManager frame_rate;
    InputHandler input;
    Renderer renderer;
    bool is_running;

public:
    PS2Emulator() : is_running(false) {}

    bool Initialize() {
        if (!graphics.Initialize()) {
            LogError("Emulator", "Failed to initialize graphics context");
            return false;
        }

        LogInfo("PS2Emulator initialized successfully");
        is_running = true;
        return true;
    }

    void Run() {
        if (!is_running) {
            LogError("Emulator", "Emulator not properly initialized");
            return;
        }

        LogInfo("Starting main loop");

        while (is_running) {
            frame_rate.StartFrame();

            // Process input
            input.ProcessEvents();
            if (input.ShouldClose()) {
                is_running = false;
                break;
            }

            // Render frame
            renderer.Clear();
            renderer.RenderFrame();
            renderer.Present(graphics.window);

            frame_rate.EndFrame();
        }

        LogInfo("Main loop ended");
    }

    ~PS2Emulator() {
        LogInfo("Shutting down PS2Emulator");
    }
};

// Entry point
int main(int argc, char* argv[]) {
    PS2Emulator emulator;

    if (!emulator.Initialize()) {
        LogError("Main", "Failed to initialize emulator");
        return EXIT_FAILURE;
    }

    emulator.Run();

    return EXIT_SUCCESS;
} 
