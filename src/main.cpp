#include <iostream>
#include "ecs.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
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

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    SDL_Surface* loaded_surface = IMG_Load("ship.png");
    SDL_Texture* ship_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    SDL_FreeSurface(loaded_surface);

    auto m = Manager();

    // Components have to be created to be used
    m.create_component<Transform>();
    m.create_component<Velocity>();
    m.create_component<Size>();
    m.create_component<Render>();
    m.create_component<Inputs>();
    m.create_component<Weapon>();
    m.create_component<Timer>();
    m.create_component<Projectile>();
    m.create_component<Collision>();
    m.create_component<Health>();
    m.create_component<Asteroid>();
    m.create_component<Rocket>();
    m.create_component<Trail>();
    m.create_component<Explode>();
    m.create_component<Fade>();
    m.create_component<Player>();
    m.create_component<AI>();
    m.create_component<MineAI>();
    m.create_component<Ship>();

    // Systems have to be created to run
    auto input_system = new InputSystem();
    m.create_system<AISystem>(new AISystem());
    m.create_system<MineAISystem>(new MineAISystem());
    m.create_system<MovementSystem>(new MovementSystem());
    m.create_system<InputSystem>(input_system);
    m.create_system<WeaponSystem>(new WeaponSystem());
    m.create_system<TimerSystem>(new TimerSystem());
    m.create_system<CollisionSystem>(new CollisionSystem());
    m.create_system<DamageSystem>(new DamageSystem());
    m.create_system<HealthSystem>(new HealthSystem());
    m.create_system<AsteroidSystem>(new AsteroidSystem());
    m.create_system<RocketSystem>(new RocketSystem());
    m.create_system<FadeSystem>(new FadeSystem());
    m.create_system<RenderSystem>(new RenderSystem(renderer, ship_texture));

    // Add the player
    Entity player_entity = m.em.get_entity();
/*
    if(player_entity != invalid_entity)
    {
        m.add_entity_component<Transform>(player_entity, Transform(RAND_BETWEEN(0.25*512, 0.75*512), RAND_BETWEEN(0.25*512, 0.75*512), 0.0));
        m.add_entity_component<Velocity>(player_entity, Velocity(0.0, RAND_BETWEEN(0, 2 * 3.142)));
        m.add_entity_component<Size>(player_entity, Size(15.0));
        m.add_entity_component<Render>(player_entity, Render(1));
        m.add_entity_component<Inputs>(player_entity, Inputs());
        m.add_entity_component<Weapon>(player_entity, Weapon());
        m.add_entity_component<Collision>(player_entity, Collision(1, true));
        m.add_entity_component<Health>(player_entity, Health(5));
        m.add_entity_component<Player>(player_entity, Player());
        m.add_entity_component<Ship>(player_entity, Ship());
    }
*/
    // Add en enemy
    for(int i = 0; i < 1; ++i)
    {
        Entity e = m.em.get_entity();
        if(e != invalid_entity)
        {
            m.add_entity_component<Transform>(e, Transform(RAND_BETWEEN(0.25*512, 0.75*512), RAND_BETWEEN(0.25*512, 0.75*512), 0.0));
            m.add_entity_component<Velocity>(e, Velocity(0.0, RAND_BETWEEN(0, 2 * 3.142)));
            m.add_entity_component<Size>(e, Size(15.0));
            m.add_entity_component<Render>(e, Render(1));
            m.add_entity_component<Inputs>(e, Inputs());
            m.add_entity_component<Weapon>(e, Weapon());
            m.add_entity_component<Collision>(e, Collision(1, true));
            m.add_entity_component<Health>(e, Health(5));
            m.add_entity_component<AI>(e, AI());
            m.add_entity_component<Ship>(e, Ship());
        }
    }

    // Add the asteroids
    for(int i = 0; i < 20; ++i)
    {
        Entity e = m.em.get_entity();
        if(e != invalid_entity)
        {
            float colour = RAND_BETWEEN(100, 200);
            m.add_entity_component<Transform>(e, Transform(RAND_BETWEEN(0, 512), RAND_BETWEEN(0, 512), RAND_BETWEEN(0, 2 * 3.142)));
            m.add_entity_component<Velocity>(e, Velocity(RAND_BETWEEN(50.0, 100.0), RAND_BETWEEN(0, 2 * 3.142)));
            m.add_entity_component<Size>(e, Size(RAND_BETWEEN(10.0, 15.0)));
            m.add_entity_component<Render>(e, Render(colour, colour, colour));
            m.add_entity_component<Collision>(e, Collision(3, false));
            m.add_entity_component<Health>(e, Health(2));
            m.add_entity_component<Asteroid>(e, Asteroid());
        }
    }

    // Add the mines
    for(int i = 0; i < 2; ++i)
    {
        Entity e = m.em.get_entity();
        if(e != invalid_entity)
        {
            m.add_entity_component<Transform>(e, Transform(RAND_BETWEEN(0, 512), RAND_BETWEEN(0, 512), RAND_BETWEEN(0, 2 * 3.142)));
            m.add_entity_component<Velocity>(e, Velocity(0.0, RAND_BETWEEN(0, 2 * 3.142)));
            m.add_entity_component<Size>(e, Size(3.0));
            m.add_entity_component<Render>(e, Render(20, 200, 20));
            m.add_entity_component<Inputs>(e, Inputs());
            m.add_entity_component<Collision>(e, Collision(3, false));
            m.add_entity_component<Health>(e, Health(1));
            m.add_entity_component<MineAI>(e, MineAI());
            m.add_entity_component<Explode>(e, Explode());
        }
    }

    // User inputs
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool use = false;
    int selected = 0;

    bool quitting = false;
    while(!quitting)
    {
        // Frame timing
        clock_t start = clock();

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

        auto a = m.get_entity_component<Inputs>(player_entity);
        if(a != nullptr)
        {
            a->left = left;
            a->right = right;
            a->up = up;
            a->down = down;
            a->use = use;
            a->mouse_x = x;
            a->mouse_y = 512 - y;
            a->selected = selected;
        }

        m.update(1.0/60);

        SDL_GL_SwapWindow(window);

        // Frame timing
        clock_t end = clock();
        double time_taken = (double)(end-start)/CLOCKS_PER_SEC;

        if(time_taken < 1.0/60)
        {
            SDL_Delay((1.0/60 - time_taken)*1000);
        }
    }

    SDL_DestroyTexture(ship_texture);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
