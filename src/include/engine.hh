
#ifndef __ENGINE_HH__
#define __ENGINE_HH__

#include "od.hh"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <array>
#include <glad/gl.h>
#include <memory_resource>
#include <print>
#include <vector>

namespace Engine 
{
    template <typename _Ty>
    struct Pos 
    {
        _Ty x, y, z;
        static struct Pos CreatePosStruct(_Ty x, _Ty y, _Ty z) noexcept {
            return Pos<_Ty>{x, y, z};
        }
    };

    template <typename _Ty>
    struct Col 
    { 
        _Ty red, green, blue, alpha; 
        static struct Col CreateColor(_Ty r, _Ty g, _Ty b, _Ty a) noexcept {
            return Col<_Ty>{r, g, b, a};
        }
    };

    [[nodiscard]]
    int create_backend(
            struct backend_version backend_ver,
            interface renderer_backend*& renderer);

    [[nodiscard]]
    int delete_backend(
            interface renderer_backend*& renderer);

    class Platform
    {
    public:
        class Window
        {
        public:
            Window(int w, int h, const char* title) {
                Platform::Get().createWindow(w, h, title, window);
                Platform::Get().setWindow(this);
            }

            ~Window() {
                Platform::Get().destroyWindow(window);
            }

            inline SDL_Window* getWindow(void) noexcept {
                return window;
            }

            inline bool swap(void) noexcept {
                return Platform::Get().glSwapWindow(window);
            }

        private:
            SDL_Window* window;
        };

        Platform() = default;
        ~Platform() = default;

        int initializePlatform(void) noexcept;

        void shutdownPlatform(void) noexcept;

        int setWindow(class Window* window) noexcept;

        int createWindow(int width, int height, const char* title, SDL_Window*& window) noexcept;

        void destroyWindow(SDL_Window*& window) noexcept;

        inline SDL_GLContext createContext(Engine::Platform::Window* window) noexcept {
            return SDL_GL_CreateContext(window->getWindow());
        }

        inline void destroyContext(SDL_GLContext& context) noexcept {
            if (context)
                SDL_GL_DestroyContext(context);
            
            context = nullptr;
        }

        inline bool setAttribute(SDL_GLAttr attrib, int val) noexcept {
            return SDL_GL_SetAttribute(attrib, val);
        }

        inline bool pollEvents(SDL_Event* e) noexcept {
            return SDL_PollEvent(e);
        }

        inline bool glSwapWindow(SDL_Window* window) noexcept {
            return SDL_GL_SwapWindow(window);
        }

        static inline Platform& Get() noexcept {
            static class Platform instance;
            return instance;
        }

    private:
        bool platform_initialized{false};
        class Window* platform_window{nullptr};
    };

    struct backend_version
    {
        int major;
        int minor;
    };

    interface renderer_backend
    {
        virtual ~renderer_backend() = default;

        virtual int init_backend(
                Platform::Window* window) = 0;

        virtual void begin_draw() = 0;
        virtual void clear_background(std::array<float, 4> rgba) = 0;

        virtual void end_draw() = 0;

        virtual void shutdown_backend() = 0;
    };

    impl backend;

    class Renderer 
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        enum class RenderingMode 
        {
            _3D,
            _2D
        };

        class Mesh 
        {
        public:
            struct Vertex 
            {
                Pos<float> position;
                Col<float> color;
            };

            explicit Mesh(std::vector<Vertex> vertx) {
                this->vertexes = vertx;
            }

            ~Mesh() = default;

        private:
            std::vector<Vertex> vertexes;
        };

        [[nodiscard]]
        inline int initializeRenderer(Platform::Window* window) noexcept {
            int status = create_backend(backend_version{
                .major = 4,
                .minor = 3
            }, this->backend);
            if (status < 0) 
                return status;

            return backend->init_backend(window);
        }

        inline void beginDraw(void) noexcept {
            backend->begin_draw();
        }

        inline void endDraw(void) noexcept {
            backend->end_draw();
        }

        inline void clearBackground(Col<float> color) noexcept {
            backend->clear_background({color.red, color.green, color.blue, color.alpha});
        }

        [[nodiscard]]
        inline int shutdownPlatformRenderer() noexcept {
            backend->shutdown_backend();
            return delete_backend(this->backend);
        }

        static inline Renderer& Get() noexcept {
            static class Renderer instance;
            return instance;
        }
    private:
        interface renderer_backend* backend;
    };

}

#endif