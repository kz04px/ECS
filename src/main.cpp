#include <iostream>
#include <cassert>
#include "ecs.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <SDL.h>
#include <SDL_opengl.h>

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

    m.createSystem<PhysicsSystem>(new PhysicsSystem());
    m.createSystem<DamageSystem>(new DamageSystem());
    m.createSystem<RenderSystem>(new RenderSystem(renderer));
    m.createSystem<RenderPlayerSystem>(new RenderPlayerSystem(renderer));
    m.createSystem<MovementSystem>(new MovementSystem());

    m.addEntityComponent(0, Position::id);
    m.addEntityComponent(0, Velocity::id);
    m.addEntityComponent(0, Render::id);
    m.addEntityComponent(0, Inputs::id);

    for(int i = 1; i < 100; ++i)
    {
        m.addEntityComponent(i, Position::id);
        m.addEntityComponent(i, Velocity::id);
        m.addEntityComponent(i, Render::id);
    }

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
        m.sm.update(0.1);

        SDL_GL_SwapWindow(window);

        SDL_Delay(2);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}