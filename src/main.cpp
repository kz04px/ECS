#include <iostream>
#include <cassert>
#include "ecs.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <SDL.h>
#include <SDL_opengl.h>
#include <ctime>

int main()
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Entity Component System Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_OPENGL);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 512;
    rect.h = 512;
    SDL_RenderSetViewport(renderer, &rect);
    SDL_RenderSetClipRect(renderer, &rect);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


    auto m = Manager();

    m.createComponent<Position>();
    m.createComponent<Velocity>();
    m.createComponent<Health>();
    m.createComponent<Render>();
    m.createComponent<Inputs>();
    m.createComponent<Size>();
    m.createComponent<Collision>();

    m.createSystem<MovementSystem>(new MovementSystem());
    m.createSystem<DamageSystem>(new DamageSystem());
    m.createSystem<RenderSystem>(new RenderSystem(renderer));
    m.createSystem<InputSystem>(new InputSystem());
    m.createSystem<DeathSystem>(new DeathSystem());
    m.createSystem<CollisionSystem>(new CollisionSystem());

    m.addEntityComponent<Position>(0, Position());
    m.addEntityComponent<Velocity>(0, Velocity());
    m.addEntityComponent<Render>(0, Render(0,0,255));
    m.addEntityComponent<Inputs>(0, Inputs());
    m.addEntityComponent<Size>(0, Size(5.0));
    m.addEntityComponent<Health>(0, Health());
    m.addEntityComponent<Collision>(0, Collision());

    for(int i = 1; i < 200; ++i)
    {
        m.addEntityComponent<Position>(i, Position());
        m.addEntityComponent<Velocity>(i, Velocity());
        m.addEntityComponent<Render>(i, Render());
        m.addEntityComponent<Size>(i, Size());
    }

#ifdef BENCHMARK
    int frames = 100000;
    clock_t start = clock();
    for(int i = 0; i < frames; ++i)
    {
        m.sm.update(0.1);
    }
    clock_t end = clock();
    double timeTaken = (double)(end-start)/CLOCKS_PER_SEC;
    double timePer = timeTaken/frames;
    std::cout << "Frames: " << frames << std::endl;
    std::cout << "Time: " << timeTaken << "s" << std::endl;
    std::cout << "30 FPS:  " << 1000*(1.0/30.0) << "ms" << std::endl;
    std::cout << "60 FPS:  " << 1000*(1.0/60.0) << "ms" << std::endl;
    std::cout << "120 FPS: " << 1000*(1.0/120.0) << "ms" << std::endl;
    std::cout << "Max FPS: " << 1/timePer << std::endl;
    std::cout << "Per frame: " << 1000*timePer << "ms" << std::endl;
#endif

    auto inputs = Inputs();
    bool quitting = false;
    while(!quitting)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    quitting = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quitting = true;
                            break;
                        case SDLK_w:
                            inputs.up = true;
                            break;
                        case SDLK_a:
                            inputs.left = true;
                            break;
                        case SDLK_s:
                            inputs.down = true;
                            break;
                        case SDLK_d:
                            inputs.right = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_w:
                            inputs.up = false;
                            break;
                        case SDLK_a:
                            inputs.left = false;
                            break;
                        case SDLK_s:
                            inputs.down = false;
                            break;
                        case SDLK_d:
                            inputs.right = false;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        m.cm.setComponent<Inputs>(inputs);
        m.sm.update(1.0/60);

        SDL_GL_SwapWindow(window);

        SDL_Delay(2);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}