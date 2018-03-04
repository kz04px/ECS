#include <iostream>
#include <cassert>
#include "ecs.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <SDL.h>
#include <SDL_opengl.h>
#include <ctime>


#define RAND_BETWEEN(a, b) ((double)rand()/RAND_MAX * (b-a) + a)


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

    // Components have to be created to be used
    m.createComponent<Position>();
    m.createComponent<Velocity>();
    m.createComponent<Health>();
    m.createComponent<Render>();
    m.createComponent<Inputs>();
    m.createComponent<Size>();
    m.createComponent<Collision>();

    // Systems have to be created to run
    m.createSystem<MovementSystem>(new MovementSystem());
    m.createSystem<DamageSystem>(new DamageSystem());
    m.createSystem<RenderSystem>(new RenderSystem(renderer));
    m.createSystem<InputSystem>(new InputSystem());
    m.createSystem<DeathSystem>(new DeathSystem());
    m.createSystem<CollisionSystem>(new CollisionSystem());

    // Add the player
    Entity e = m.em.getEntity();
    if(e != invalidEntity)
    {
        m.addEntityComponent<Position>(e, Position(RAND_BETWEEN(0.25*512, 0.75*512), RAND_BETWEEN(0.25*512, 0.75*512)));
        m.addEntityComponent<Velocity>(e, Velocity(8.0, 0.0));
        m.addEntityComponent<Render>(e, Render(0,0,255));
        m.addEntityComponent<Inputs>(e, Inputs());
        m.addEntityComponent<Size>(e, Size(5.0));
        m.addEntityComponent<Health>(e, Health());
        m.addEntityComponent<Collision>(e, Collision());
    }

    // Add the asteroids
    for(int i = 0; i < 200; ++i)
    {
        Entity e = m.em.getEntity();
        if(e != invalidEntity)
        {
            m.addEntityComponent<Position>(e, Position(RAND_BETWEEN(0, 512), RAND_BETWEEN(0, 512)));
            m.addEntityComponent<Velocity>(e, Velocity(RAND_BETWEEN(5.0, 10.0), RAND_BETWEEN(0, 2 * 3.142)));
            m.addEntityComponent<Render>(e, Render(RAND_BETWEEN(200, 255), RAND_BETWEEN(200, 255), RAND_BETWEEN(200, 255)));
            m.addEntityComponent<Size>(e, Size(RAND_BETWEEN(3.0, 4.0)));
        }
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