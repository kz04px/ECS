#include <iostream>
#include <cassert>
#include "ecs.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <SDL.h>
#include <SDL_opengl.h>

int main()
{
    auto m = Manager();

    m.createComponent<Position>();
    m.createComponent<Velocity>();
    m.createComponent<Health>();
    m.createComponent<Render>();

    m.createSystem<PhysicsSystem>();
    m.createSystem<DamageSystem>();
    //m.createSystem<RenderSystem>();

    m.addEntityComponent(0, Position::id);
    m.addEntityComponent(0, Velocity::id);
    m.addEntityComponent(0, Health::id);
    m.addEntityComponent(0, Render::id);

    m.addEntityComponent(1, Position::id);
    m.addEntityComponent(1, Velocity::id);
    m.addEntityComponent(1, Render::id);

    m.print();

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Entity Component System Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_OPENGL);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
/*
    auto a = m.sm.getSystem<RenderSystem>();
    if(a != NULL)
    {
        a->renderer = renderer;
    }
    std::cout << "REEE" << std::endl;
*/
    bool quitting = false;
    while(!quitting)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                quitting = true;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        m.sm.update(0.1);

        SDL_GL_SwapWindow(window);

        SDL_Delay(2);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}