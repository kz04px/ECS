#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "ecs.hpp"
#include <SDL.h>

class MovementSystem : public System
{
    public:
        MovementSystem()
        {
            required.insert(Transform::id);
            required.insert(Velocity::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &transform_store = manager->cm.get_store<Transform>();
            auto &velocity_store = manager->cm.get_store<Velocity>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Transform::id));
                assert(manager->cm.entity_has_component(e, Velocity::id));

                auto transform = transform_store.get_component(e);
                auto velocity = velocity_store.get_component(e);

                transform->x += dt * velocity->x;
                transform->y += dt * velocity->y;

                if(transform->x > 512) {transform->x -= 512;}
                if(transform->x <   0) {transform->x += 512;}

                if(transform->y > 512) {transform->y -= 512;}
                if(transform->y <   0) {transform->y += 512;}
            }
        }
    private:
};

class RenderSystem : public System
{
    public:
        RenderSystem(SDL_Renderer *r, SDL_Texture *ship_texture) : renderer(r), ship_texture(ship_texture)
        {
            required.insert(Transform::id);
            required.insert(Render::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);
            assert(renderer != nullptr);
            assert(ship_texture != nullptr);

            auto &transform_store = manager->cm.get_store<Transform>();
            auto &size_store = manager->cm.get_store<Size>();
            auto &render_store = manager->cm.get_store<Render>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Transform::id));
                assert(manager->cm.entity_has_component(e, Render::id));
                assert(manager->cm.entity_has_component(e, Size::id));

                auto a = transform_store.get_component(e);
                auto b = size_store.get_component(e);
                auto c = render_store.get_component(e);

                SDL_SetRenderDrawColor(renderer, c->red, c->green, c->blue, c->alpha);

                for(int x = -1; x < 2; ++x)
                {
                    for(int y = -1; y < 2; ++y)
                    {
                        SDL_Rect rect;
                        rect.x = a->x - b->radius + x*512;
                        rect.y = 512 - a->y - b->radius + y*512;
                        rect.w = 2 * b->radius;
                        rect.h = 2 * b->radius;

                        if(c->texture == 1)
                        {
                            SDL_Point center = {(int)b->radius, (int)b->radius};
                            SDL_RenderCopyEx(renderer, ship_texture, nullptr, &rect, -a->rotation*180/3.142 + 90, &center, SDL_FLIP_NONE);
                        }
                        else
                        {
                            SDL_RenderFillRect(renderer, &rect);
                        }
                    }
                }
            }
        }
    private:
        SDL_Renderer *renderer;
        SDL_Texture *ship_texture;
};

class InputSystem : public System
{
    public:
        InputSystem()
        {
            required.insert(Transform::id);
            required.insert(Velocity::id);
            required.insert(Inputs::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &transform_store = manager->cm.get_store<Transform>();
            auto &velocity_store = manager->cm.get_store<Velocity>();
            auto &inputs_store   = manager->cm.get_store<Inputs>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Transform::id));
                assert(manager->cm.entity_has_component(e, Velocity::id));
                assert(manager->cm.entity_has_component(e, Inputs::id));

                auto transform = transform_store.get_component(e);
                auto inputs = inputs_store.get_component(e);
                auto velocity = velocity_store.get_component(e);

                float dx = inputs->mouse_x - transform->x;
                float dy = inputs->mouse_y - transform->y;
                transform->rotation = atan2(dy, dx);

                if(inputs->left == true)       {velocity->x -= 5;}
                else if(inputs->right == true) {velocity->x += 5;}
                if(inputs->up == true)         {velocity->y += 5;}
                else if(inputs->down == true)  {velocity->y -= 5;}

                const float speed_limit = 100.0;
                if(velocity->x > speed_limit)       {velocity->x =  speed_limit;}
                else if(velocity->x < -speed_limit) {velocity->x = -speed_limit;}
                if(velocity->y > speed_limit)       {velocity->y =  speed_limit;}
                else if(velocity->y < -speed_limit) {velocity->y = -speed_limit;}
            }
        }
    private:
};

class WeaponSystem : public System
{
    public:
        WeaponSystem()
        {
            required.insert(Weapon::id);
            required.insert(Inputs::id);
            required.insert(Transform::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &inputs_store = manager->cm.get_store<Inputs>();
            auto &transform_store = manager->cm.get_store<Transform>();
            auto &weapon_store = manager->cm.get_store<Weapon>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Weapon::id));
                assert(manager->cm.entity_has_component(e, Inputs::id));
                assert(manager->cm.entity_has_component(e, Transform::id));

                auto a = inputs_store.get_component(e);
                auto b = weapon_store.get_component(e);

                b->time_left -= dt;

                if(b->time_left < 0.0)
                {
                    b->time_left = 0.0;
                }

                if(a->use == true && b->time_left <= 0.0)
                {
                    b->time_left = 0.1;

                    Entity new_entity = manager->em.get_entity();
                    if(new_entity != invalid_entity)
                    {
                        auto transform = transform_store.get_component(e);

                        float x = transform->x + 25.0*cos(transform->rotation);
                        float y = transform->y + 25.0*sin(transform->rotation);

                        if(a->selected == 0)
                        {
                            // Bullet
                            manager->add_entity_component<Transform>(new_entity, Transform(x, y, transform->rotation));
                            manager->add_entity_component<Velocity>(new_entity, Velocity(200.0, transform->rotation));
                            manager->add_entity_component<Render>(new_entity, Render(0,255,0));
                            manager->add_entity_component<Size>(new_entity, Size(1.0));
                            manager->add_entity_component<Timer>(new_entity, Timer(1.0));
                            manager->add_entity_component<Projectile>(new_entity, Projectile(e, 1));
                            manager->add_entity_component<Collision>(new_entity, Collision(2, true));
                            manager->add_entity_component<Health>(new_entity, Health());
                        }
                        else if(a->selected == 1)
                        {
                            // Rocket
                            manager->add_entity_component<Transform>(new_entity, Transform(x, y, transform->rotation));
                            manager->add_entity_component<Velocity>(new_entity, Velocity(50.0, transform->rotation));
                            manager->add_entity_component<Render>(new_entity, Render(255,0,0));
                            manager->add_entity_component<Size>(new_entity, Size(2.0));
                            manager->add_entity_component<Timer>(new_entity, Timer(2.0));
                            manager->add_entity_component<Rocket>(new_entity, Rocket(e, 2, 0.5));
                            manager->add_entity_component<Collision>(new_entity, Collision(2, true));
                            manager->add_entity_component<Health>(new_entity, Health());
                            manager->add_entity_component<Explode>(new_entity, Explode());
                        }
                    }
                }
            }
        }
    private:
};

class RocketSystem : public System
{
    public:
        RocketSystem()
        {
            required.insert(Rocket::id);
            required.insert(Transform::id);
            required.insert(Velocity::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &rocket_store = manager->cm.get_store<Rocket>();
            auto &transform_store = manager->cm.get_store<Transform>();
            auto &velocity_store = manager->cm.get_store<Velocity>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Rocket::id));
                assert(manager->cm.entity_has_component(e, Transform::id));
                assert(manager->cm.entity_has_component(e, Velocity::id));

                auto r = rocket_store.get_component(e);

                r->boost_time_left -= dt;

                if(r->boost_time_left <= 0.0 && r->boost_time_left + dt > 0.0)
                {
                    auto v = velocity_store.get_component(e);
                    v->x *= 5;
                    v->y *= 5;
                    manager->add_entity_component<Trail>(e, Trail());
                }

                if(manager->cm.entity_has_component(e, Trail::id))
                {
                    if(rand()%2 == 0)
                    {
                        Entity new_entity = manager->em.get_entity();
                        if(new_entity != invalid_entity)
                        {
                            auto transform = transform_store.get_component(e);

                            manager->add_entity_component<Transform>(new_entity, Transform(
                                transform->x + RAND_BETWEEN(-3.0, 3.0),
                                transform->y + RAND_BETWEEN(-3.0, 3.0),
                                RAND_BETWEEN(0, 2 * 3.142))
                            );
                            manager->add_entity_component<Size>(new_entity, Size(1.0));
                            int n = rand()%4;
                            if(n == 0)
                            {
                                manager->add_entity_component<Render>(new_entity, Render(220, 20, 20));
                            }
                            else if(n == 1)
                            {
                                manager->add_entity_component<Render>(new_entity, Render(220, 200, 20));
                            }
                            else if(n == 2)
                            {
                                manager->add_entity_component<Render>(new_entity, Render(220, 70, 20));
                            }
                            else
                            {
                                manager->add_entity_component<Render>(new_entity, Render(50, 20, 20));
                            }
                            float time = RAND_BETWEEN(0.5, 0.75);
                            manager->add_entity_component<Timer>(new_entity, Timer(time));
                            manager->add_entity_component<Fade>(new_entity, Fade(time));
                        }
                    }
                }
            }
        }
    private:
};

class TimerSystem : public System
{
    public:
        TimerSystem()
        {
            required.insert(Timer::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &timer_store = manager->cm.get_store<Timer>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Timer::id));

                auto a = timer_store.get_component(e);

                a->time_left -= dt;

                if(a->time_left <= 0.0)
                {
                    manager->remove.push_back(e);
                }
            }
        }
    private:
};

class CollisionSystem : public System
{
    public:
        CollisionSystem()
        {
            required.insert(Collision::id);
            required.insert(Transform::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &collision_store = manager->cm.get_store<Collision>();
            auto &transform_store = manager->cm.get_store<Transform>();
            auto &size_store = manager->cm.get_store<Size>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Collision::id));
                assert(manager->cm.entity_has_component(e, Transform::id));
                assert(manager->cm.entity_has_component(e, Size::id));

                auto c = collision_store.get_component(e);
                auto a = transform_store.get_component(e);
                auto r = size_store.get_component(e);

                c->collided = false;

                for(auto e2 : entities)
                {
                    if(e == e2) {continue;} // FIXME: Try e <= e2

                    auto c2 = collision_store.get_component(e2);

                    // FIXME: Test this
                    if(c->mask == c2->mask && c->self == false) {continue;}
                    //if((c->mask & c2->mask) == 0) {continue;}

                    auto a2 = transform_store.get_component(e2);
                    auto r2 = size_store.get_component(e2);

                    float dx = a->x - a2->x;
                    float dy = a->y - a2->y;
                    float dist = r->radius + r2->radius;

                    if(fabs(dx) <= dist && fabs(dy) <= dist)
                    {
                        c->collided = true;
                        break;
                    }
                }
            }
        }
    private:
};

class HealthSystem : public System
{
    public:
        HealthSystem()
        {
            required.insert(Health::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &health_store = manager->cm.get_store<Health>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Health::id));

                auto h = health_store.get_component(e);

                h->immunity -= dt;
                if(h->immunity < 0.0)
                {
                    h->immunity = 0.0;
                }
            }
        }
    private:
};

class DamageSystem : public System
{
    public:
        DamageSystem()
        {
            required.insert(Health::id);
            required.insert(Collision::id);
            required.insert(Transform::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &collision_store = manager->cm.get_store<Collision>();
            auto &health_store = manager->cm.get_store<Health>();
            auto &transform_store = manager->cm.get_store<Transform>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Collision::id));
                assert(manager->cm.entity_has_component(e, Health::id));
                assert(manager->cm.entity_has_component(e, Transform::id));

                auto c = collision_store.get_component(e);
                auto h = health_store.get_component(e);

                if(c->collided == true && h->immunity <= 0.0)
                {
                    h->health--;
                    h->immunity = 0.1;

                    if(h->health <= 0)
                    {
                        manager->remove.push_back(e);

                        if(manager->cm.entity_has_component(e, Explode::id))
                        {
                            for(int i = 0; i < 20; ++i)
                            {
                                Entity new_entity = manager->em.get_entity();
                                if(new_entity != invalid_entity)
                                {
                                    auto transform = transform_store.get_component(e);

                                    manager->add_entity_component<Transform>(new_entity, Transform(
                                        transform->x + RAND_BETWEEN(-4.0, 4.0)*RAND_BETWEEN(-4.0, 4.0),
                                        transform->y + RAND_BETWEEN(-4.0, 4.0)*RAND_BETWEEN(-4.0, 4.0),
                                        RAND_BETWEEN(0, 2 * 3.142))
                                    );
                                    manager->add_entity_component<Size>(new_entity, Size(5.0));
                                    int n = rand()%4;
                                    if(n == 0)
                                    {
                                        manager->add_entity_component<Render>(new_entity, Render(220, 20, 20));
                                    }
                                    else if(n == 1)
                                    {
                                        manager->add_entity_component<Render>(new_entity, Render(220, 200, 20));
                                    }
                                    else if(n == 2)
                                    {
                                        manager->add_entity_component<Render>(new_entity, Render(220, 70, 20));
                                    }
                                    else
                                    {
                                        manager->add_entity_component<Render>(new_entity, Render(50, 20, 20));
                                    }
                                    float time = RAND_BETWEEN(0.1, 1.0);
                                    manager->add_entity_component<Timer>(new_entity, Timer(time));
                                    manager->add_entity_component<Fade>(new_entity, Fade(time));
                                }
                            }
                        }
                    }
                }
            }
        }
    private:
};

class AsteroidSystem : public System
{
    public:
        AsteroidSystem()
        {
            required.insert(Transform::id);
            required.insert(Size::id);
            required.insert(Health::id);
            required.insert(Asteroid::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &transform_store = manager->cm.get_store<Transform>();
            auto &size_store = manager->cm.get_store<Size>();
            auto &health_store = manager->cm.get_store<Health>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Transform::id));
                assert(manager->cm.entity_has_component(e, Size::id));
                assert(manager->cm.entity_has_component(e, Health::id));

                auto health = health_store.get_component(e);
                auto size = size_store.get_component(e);

                if(health->health <= 0 && size->radius >= 6.0)
                {
                    auto transform = transform_store.get_component(e);

                    // New asteroids
                    for(int i = 0; i < rand()%2+3; ++i)
                    {
                        Entity new_entity = manager->em.get_entity();
                        if(new_entity != invalid_entity)
                        {
                            int colour = RAND_BETWEEN(100, 200);
                            manager->add_entity_component<Transform>(new_entity, Transform(transform->x, transform->y, RAND_BETWEEN(0, 2 * 3.142)));
                            manager->add_entity_component<Velocity>(new_entity, Velocity(RAND_BETWEEN(50.0, 100.0), RAND_BETWEEN(0, 2 * 3.142)));
                            manager->add_entity_component<Size>(new_entity, Size(size->radius/2));
                            manager->add_entity_component<Render>(new_entity, Render(colour, colour, colour));
                            manager->add_entity_component<Collision>(new_entity, Collision(3, false));
                            manager->add_entity_component<Health>(new_entity, Health(health->startHealth - 1));
                            manager->add_entity_component<Asteroid>(new_entity, Asteroid());
                        }
                    }

                    // Pretty particles
                    for(int i = 0; i < rand()%5+20; ++i)
                    {
                        Entity new_entity = manager->em.get_entity();
                        if(new_entity != invalid_entity)
                        {
                            manager->add_entity_component<Transform>(new_entity, Transform(transform->x, transform->y, RAND_BETWEEN(0, 2 * 3.142)));
                            manager->add_entity_component<Velocity>(new_entity, Velocity(RAND_BETWEEN(150.0, 300.0), RAND_BETWEEN(0, 2 * 3.142)));
                            manager->add_entity_component<Size>(new_entity, Size(1.0));
                            if(rand()%2 == 0)
                            {
                                manager->add_entity_component<Render>(new_entity, Render(220, 20, 20));
                            }
                            else
                            {
                                manager->add_entity_component<Render>(new_entity, Render(220, 140, 20));
                            }
                            manager->add_entity_component<Timer>(new_entity, Timer(0.5));
                            manager->add_entity_component<Fade>(new_entity, Fade(0.5));
                        }
                    }
                }
            }
        }
    private:
};

class FadeSystem : public System
{
    public:
        FadeSystem()
        {
            required.insert(Fade::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &fade_store = manager->cm.get_store<Fade>();
            auto &render_store = manager->cm.get_store<Render>();

            for(auto e : entities)
            {
                assert(manager->cm.entity_has_component(e, Fade::id));

                auto fade = fade_store.get_component(e);
                auto render = render_store.get_component(e);

                fade->time += dt;

                if(fade->time < fade->fade_time)
                {
                    render->alpha = 255*(1.0 - fade->time / fade->fade_time);
                }
                else
                {
                    render->alpha = 0;
                }
            }
        }
    private:
};

class AISystem : public System
{
    public:
        AISystem()
        {
            required.insert(AI::id);
            required.insert(Inputs::id);
            required.insert(Transform::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &ai_store = manager->cm.get_store<AI>();
            auto &inputs_store = manager->cm.get_store<Inputs>();
            auto &transform_store = manager->cm.get_store<Transform>();
            auto &velocity_store = manager->cm.get_store<Velocity>();

            auto players = manager->cm.components[Player::id];
            auto asteroids = manager->cm.components[Asteroid::id];

            for(auto e : entities)
            {
                auto transform1 = transform_store.get_component(e);
                auto inputs = inputs_store.get_component(e);
                auto ai = ai_store.get_component(e);
                auto velocity = velocity_store.get_component(e);

                // Reset inputs
                inputs->up = false;
                inputs->down = false;
                inputs->left = false;
                inputs->right = false;
                inputs->selected = 0;
                inputs->use = false;

                float closest_dist = 1000000;
                float closest_x = 0.0;
                float closest_y = 0.0;

                Entity closest_player = invalid_entity;
                Entity closest_asteroid = invalid_entity;

                ai->timer += dt;

                if(ai->aggressive == true)
                {
                    for(auto p : players)
                    {
                        auto transform2 = transform_store.get_component(p);

                        float dx = transform2->x - transform1->x;
                        //if(fabs(dx) > 200.0) {continue;}
                        float dy = transform2->y - transform1->y;
                        //if(fabs(dy) > 200.0) {continue;}

                        float dist = sqrt(dx*dx + dy*dy);
                        if(dist < closest_dist)
                        {
                            closest_dist = dist;
                            closest_x = transform2->x;
                            closest_y = transform2->y;
                            closest_player = p;
                        }
                    }
                }

                for(auto a : asteroids)
                {
                    auto transform2 = transform_store.get_component(a);

                    float dx = transform2->x - transform1->x;
                    //if(fabs(dx) > 200.0) {continue;}
                    float dy = transform2->y - transform1->y;
                    //if(fabs(dy) > 200.0) {continue;}

                    float dist = sqrt(dx*dx + dy*dy);
                    if(dist < closest_dist)
                    {
                        closest_dist = dist;
                        closest_x = transform2->x;
                        closest_y = transform2->y;
                        closest_asteroid = a;
                    }
                }

                float barrel = 25.0;
                if(closest_asteroid != invalid_entity && closest_dist >= barrel)
                {
                    // Predict where we're aiming
                    auto asteroid_transform = transform_store.get_component(closest_asteroid);
                    auto asteroid_velocity = velocity_store.get_component(closest_asteroid);

                    float ship_x = transform1->x;
                    float ship_y = transform1->y;

                    float asteroid_x = asteroid_transform->x;
                    float asteroid_y = asteroid_transform->y;

                    float asteroid_vx = asteroid_velocity->x;
                    float asteroid_vy = asteroid_velocity->y;

                    float bullet_speed = 200;

                    float a = asteroid_vx*asteroid_vx + asteroid_vy*asteroid_vy - bullet_speed*bullet_speed;
                    if(a != 0.0)
                    {
                        float b = 2*(asteroid_vx * (asteroid_x - ship_x) + asteroid_vy * (asteroid_y - ship_y) - bullet_speed*barrel);
                        float c = (asteroid_x-ship_x)*(asteroid_x-ship_x) + (asteroid_y-ship_y)*(asteroid_y-ship_y) - barrel*barrel;

                        float disc = b*b - 4*a*c;

                        float t1 = (-b + sqrt(disc)) / (2 * a);
                        float t2 = (-b - sqrt(disc)) / (2 * a);

                        float t = (t1 < t2 ? t1 : t2);
                        if(t < 0.0) {t = (t1 > t2 ? t1 : t2);}

                        float aim_x = asteroid_x + t * asteroid_vx;
                        float aim_y = asteroid_y + t * asteroid_vy;
                        float aim_dist = sqrt((aim_x-ship_x)*(aim_x-ship_x) + (aim_y-ship_y)*(aim_y-ship_y));

                        inputs->mouse_x = aim_x;
                        inputs->mouse_y = aim_y;

                        if(aim_dist <= 200.0)
                        {
                            inputs->selected = 0;
                            inputs->use = true;
                        }

                        Entity new_entity = manager->em.get_entity();
                        if(new_entity != invalid_entity)
                        {
                            manager->add_entity_component<Transform>(new_entity, Transform(aim_x, aim_y, 0.0));
                            manager->add_entity_component<Size>(new_entity, Size(3.0));
                            manager->add_entity_component<Render>(new_entity, Render(220, 20, 20));
                            manager->remove.push_back(new_entity);
                        }
                    }
                }

                float dx = closest_x - transform1->x;
                float dy = closest_y - transform1->y;

                dx = (dx > 512/2 ? 512-dx : dx);
                dy = (dy > 512/2 ? 512-dy : dy);

                if(dx > 100)
                {
                    if(velocity->x < 100.0)
                    {
                        inputs->right = true;
                    }
                    else
                    {
                        inputs->left = true;
                    }
                }
                else if(dx < -100)
                {
                    if(velocity->x > -100.0)
                    {
                        inputs->left = true;
                    }
                    else
                    {
                        inputs->right = true;
                    }
                }
                else if(dx > 0)
                {
                    inputs->left = true;
                }
                else if(dx < 0)
                {
                    inputs->right = true;
                }

                if(dy > 100)
                {
                    if(velocity->y < 100.0)
                    {
                        inputs->up = true;
                    }
                    else
                    {
                        inputs->down = true;
                    }
                }
                else if(dy < -100)
                {
                    if(velocity->y > -100.0)
                    {
                        inputs->down = true;
                    }
                    else
                    {
                        inputs->up = true;
                    }
                }
                else if(dy > 0)
                {
                    inputs->down = true;
                }
                else if(dy < 0)
                {
                    inputs->up = true;
                }
            }
        }
    private:
};

class MineAISystem : public System
{
    public:
        MineAISystem()
        {
            required.insert(MineAI::id);
        }
        void update(const float dt)
        {
            assert(manager != nullptr);

            auto &mine_ai_store = manager->cm.get_store<MineAI>();
            auto &inputs_store = manager->cm.get_store<Inputs>();
            auto &transform_store = manager->cm.get_store<Transform>();

            auto ships = manager->cm.components[Ship::id];
            for(auto e : entities)
            {
                auto mine_ai = mine_ai_store.get_component(e);
                auto inputs = inputs_store.get_component(e);

                // Reset inputs
                inputs->up = false;
                inputs->down = false;
                inputs->left = false;
                inputs->right = false;
                inputs->selected = 0;
                inputs->use = false;

                if(mine_ai->aggressive == false)
                {
                    continue;
                }

                float closest_dx = 0.0;
                float closest_dy = 0.0;
                float closest_dist = 1000000;

                auto transform1 = transform_store.get_component(e);
                for(auto s : ships)
                {
                    auto transform2 = transform_store.get_component(s);

                    float dx = transform2->x - transform1->x;
                    float dy = transform2->y - transform1->y;

                    float dist = sqrt(dx*dx + dy*dy);
                    if(dist < closest_dist)
                    {
                        closest_dx = dx;
                        closest_dy = dy;
                        closest_dist = dist;
                    }
                }

                if(closest_dist <= 200.0)
                {
                    if(closest_dx > 0.0)
                    {
                        inputs->right = true;
                    }
                    else
                    {
                        inputs->left = true;
                    }

                    if(closest_dy < 0.0)
                    {
                        inputs->down = true;
                    }
                    else
                    {
                        inputs->up = true;
                    }
                }
            }
        }
    private:
};

#endif
