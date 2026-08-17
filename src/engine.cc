#include "include/engine.hh"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <cwchar>
#include <glad/gl.h>

int Engine::Platform::initializePlatform(void) noexcept {
    if (platform_initialized == true) {
        std::print("Platform is already initialized\n");
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -15;
    }

    platform_initialized = true;
    return 0;
}

int Engine::Platform::setWindow(class Window* wnd) noexcept {
    if (wnd == nullptr)
        return -1;

    this->platform_window = wnd;
    return 0;
}

int Engine::Platform::createWindow(int width, int height, const char* title, SDL_Window*& window) noexcept {
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
    if (!window)
        return -19;

    return 0;
}

void Engine::Platform::destroyWindow(SDL_Window*& window) noexcept {
    if (window)
        SDL_DestroyWindow(window);
    
    window = nullptr;
}

void Engine::Platform::shutdownPlatform(void) noexcept {
    if (!platform_initialized)
        return;

    SDL_Quit();
    platform_initialized = false;
}

impl Engine::backend : public Engine::renderer_backend 
{
public:
    Engine::backend_version ver{};
    SDL_GLContext context{nullptr};
    bool draw{false};

    backend(Engine::backend_version v) : ver(v) {}

    int init_backend(Engine::Platform::Window* wnd) override {
        auto& pf = Engine::Platform::Get();

        pf.setAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ver.major);
        pf.setAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ver.minor);
        pf.setAttribute(SDL_GL_DEPTH_SIZE, 24);

        context = pf.createContext(wnd);
        if (!context)
            return -4;

        if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress))
            return -9;
        return 0;
    }

    void begin_draw() override {
        draw = true;
    }

    void end_draw() override {
        draw = false;
    }

    void clear_background(std::array<float, 4> rgba) override {
        if (draw == true) { 
            glClearColor(rgba.at(0), rgba.at(1), rgba.at(2), rgba.at(3));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }

    void shutdown_backend() override {
        Engine::Platform::Get().destroyContext(context);
    }
};

[[nodiscard]]
int Engine::create_backend(
        struct Engine::backend_version backend_ver,
        interface Engine::renderer_backend*& renderer) 
{
    renderer = new Engine::backend(backend_ver);
    if (!renderer) 
        return -1;

    return 0;
}

[[nodiscard]]
int Engine::delete_backend(
        interface renderer_backend*& renderer) 
{
    if (renderer == nullptr) 
        return 1;

    delete renderer;
    return 0;
}