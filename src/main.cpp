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
    //SDL_RenderPresent(renderer);
    SDL_RenderSetViewport(renderer, &rect);
    SDL_RenderSetClipRect(renderer, &rect);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


    auto m = Manager();

    m.createComponent<Position>();
    m.createComponent<Velocity>();
    m.createComponent<Health>();
    m.createComponent<Render>();

    m.createSystem<PhysicsSystem>(new PhysicsSystem());
    m.createSystem<DamageSystem>(new DamageSystem());
    m.createSystem<RenderSystem>(new RenderSystem(renderer));

    for(int i = 0; i < 100; ++i)
    {
        m.addEntityComponent(i, Position::id);
        m.addEntityComponent(i, Velocity::id);
        m.addEntityComponent(i, Render::id);
    }

    //m.print();


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