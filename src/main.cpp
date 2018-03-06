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
    m.createComponent<Size>();
    m.createComponent<Render>();
    m.createComponent<Inputs>();
    m.createComponent<Weapon>();
    m.createComponent<Timer>();
    m.createComponent<Remove>();
    m.createComponent<Projectile>();
    m.createComponent<Collision>();
    m.createComponent<Health>();
    m.createComponent<Asteroid>();

    // Systems have to be created to run
    auto inputSystem = new InputSystem();
    m.createSystem<MovementSystem>(new MovementSystem());
    m.createSystem<RenderSystem>(new RenderSystem(renderer));
    m.createSystem<InputSystem>(inputSystem);
    m.createSystem<RemoveSystem>(new RemoveSystem());
    m.createSystem<WeaponSystem>(new WeaponSystem());
    m.createSystem<TimerSystem>(new TimerSystem());
    m.createSystem<CollisionSystem>(new CollisionSystem());
    m.createSystem<DamageSystem>(new DamageSystem());
    m.createSystem<HealthSystem>(new HealthSystem());
    m.createSystem<AsteroidSystem>(new AsteroidSystem());

    // Add the player
    Entity playerEntity = m.em.getEntity();
    if(playerEntity != invalidEntity)
    {
        m.addEntityComponent<Position>(playerEntity, Position(RAND_BETWEEN(0.25*512, 0.75*512), RAND_BETWEEN(0.25*512, 0.75*512)));
        m.addEntityComponent<Velocity>(playerEntity, Velocity(8.0, 0.0));
        m.addEntityComponent<Size>(playerEntity, Size(5.0));
        m.addEntityComponent<Render>(playerEntity, Render(0,0,255));
        m.addEntityComponent<Inputs>(playerEntity, Inputs());
        m.addEntityComponent<Weapon>(playerEntity, Weapon());
        m.addEntityComponent<Collision>(playerEntity, Collision(1, true));
        m.addEntityComponent<Health>(playerEntity, Health(3));
        m.addEntityComponent<Remove>(playerEntity, Remove());
    }

    // Add the asteroids
    for(int i = 0; i < 20; ++i)
    {
        Entity e = m.em.getEntity();
        if(e != invalidEntity)
        {
            float colour = RAND_BETWEEN(100, 200);
            m.addEntityComponent<Position>(e, Position(RAND_BETWEEN(0, 512), RAND_BETWEEN(0, 512)));
            m.addEntityComponent<Velocity>(e, Velocity(RAND_BETWEEN(5.0, 10.0), RAND_BETWEEN(0, 2 * 3.142)));
            m.addEntityComponent<Size>(e, Size(RAND_BETWEEN(10.0, 15.0)));
            m.addEntityComponent<Render>(e, Render(colour, colour, colour));
            m.addEntityComponent<Collision>(e, Collision(3, false));
            m.addEntityComponent<Health>(e, Health(2));
            m.addEntityComponent<Remove>(e, Remove());
            m.addEntityComponent<Asteroid>(e, Asteroid());
        }
    }


    // User inputs
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool use = false;
    int selected = 0;


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
                            up = true;
                            break;
                        case SDLK_a:
                            left = true;
                            break;
                        case SDLK_s:
                            down = true;
                            break;
                        case SDLK_d:
                            right = true;
                            break;
                        case SDLK_1:
                            std::cout << "Selected: 1" << std::endl;
                            selected = 0;
                            break;
                        case SDLK_2:
                            std::cout << "Selected: 2" << std::endl;
                            selected = 1;
                            break;
                        case SDLK_3:
                            std::cout << "Selected: 3" << std::endl;
                            selected = 2;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_w:
                            up = false;
                            break;
                        case SDLK_a:
                            left = false;
                            break;
                        case SDLK_s:
                            down = false;
                            break;
                        case SDLK_d:
                            right = false;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            use = true;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            use = false;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        int x;
        int y;
        SDL_GetMouseState(&x, &y);

        glClear(GL_COLOR_BUFFER_BIT);

        auto a = m.getEntityComponent<Inputs>(playerEntity);
        a->left = left;
        a->right = right;
        a->up = up;
        a->down = down;
        a->use = use;
        a->mouseX = x;
        a->mouseY = y;
        a->selected = selected;

        m.sm.update(1.0/60);

        SDL_GL_SwapWindow(window);

        SDL_Delay(2);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}