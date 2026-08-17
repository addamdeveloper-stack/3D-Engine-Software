#include "src/include/engine.hh"

int main() {
    auto& pf = Engine::Platform::Get();
    int success = 0;
    if ((success = pf.initializePlatform()) < 0) return success;

    {
        bool running = true;
        Engine::Platform::Window window(1100, 670, "MyWindow");
        Engine::Renderer& renderer = Engine::Renderer::Get();
        if ((success = renderer.initializeRenderer(&window)) < 0) 
            return success;

        SDL_Event event;
        Engine::Col<float> bgColor = Engine::Col<float>::CreateColor(255.0f, 0.0f, 0.0f, 255.0f);
        while (running)
        {
            while (pf.pollEvents(&event)) {
                if (event.type == SDL_EventType::SDL_EVENT_QUIT) 
                    running = false;
            }

            renderer.beginDraw();
            renderer.clearBackground(bgColor);



            renderer.endDraw();
            window.swap();
        }

        if ((success = renderer.shutdownPlatformRenderer()) < 0) 
            return success;
    }

    pf.shutdownPlatform();
    return 0;
}