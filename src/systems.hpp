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
            assert(manager != NULL);

            auto &transformStore = manager->cm.getStore<Transform>();
            auto &velocityStore = manager->cm.getStore<Velocity>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Transform::id));
                assert(manager->cm.entityHasComponent(e, Velocity::id));

                auto transform = transformStore.getComponent(e);
                auto velocity = velocityStore.getComponent(e);

                transform->x += dt * velocity->speed * velocity->x;
                transform->y += dt * velocity->speed * velocity->y;

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
        RenderSystem(SDL_Renderer *r, SDL_Texture *shipTexture) : renderer(r), shipTexture(shipTexture)
        {
            required.insert(Transform::id);
            required.insert(Render::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);
            assert(renderer != NULL);
            assert(shipTexture != NULL);

            auto &transformStore = manager->cm.getStore<Transform>();
            auto &sizeStore = manager->cm.getStore<Size>();
            auto &renderStore = manager->cm.getStore<Render>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Transform::id));
                assert(manager->cm.entityHasComponent(e, Render::id));
                assert(manager->cm.entityHasComponent(e, Size::id));

                auto a = transformStore.getComponent(e);
                auto b = sizeStore.getComponent(e);
                auto c = renderStore.getComponent(e);

                SDL_SetRenderDrawColor(renderer, c->red, c->green, c->blue, 255);

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
                            SDL_Point center = {b->radius, b->radius};
                            SDL_RenderCopyEx(renderer, shipTexture, NULL, &rect, -a->rotation*180/3.142 + 90, &center, SDL_FLIP_NONE);
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
        SDL_Texture *shipTexture;
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
            assert(manager != NULL);

            auto &transformStore = manager->cm.getStore<Transform>();
            auto &velocityStore = manager->cm.getStore<Velocity>();
            auto &inputsStore   = manager->cm.getStore<Inputs>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Transform::id));
                assert(manager->cm.entityHasComponent(e, Velocity::id));
                assert(manager->cm.entityHasComponent(e, Inputs::id));

                auto transform = transformStore.getComponent(e);
                auto inputs = inputsStore.getComponent(e);
                auto velocity = velocityStore.getComponent(e);

                float dx = inputs->mouseX - transform->x;
                float dy = inputs->mouseY - transform->y;
                transform->rotation = atan2(dy, dx);

                if(inputs->left == true)       {velocity->x -= 0.05;}
                else if(inputs->right == true) {velocity->x += 0.05;}
                if(inputs->up == true)         {velocity->y += 0.05;}
                else if(inputs->down == true)  {velocity->y -= 0.05;}

                if(velocity->x > 5.0)       {velocity->x =  5.0;}
                else if(velocity->x < -5.0) {velocity->x = -5.0;}
                if(velocity->y > 5.0)       {velocity->y =  5.0;}
                else if(velocity->y < -5.0) {velocity->y = -5.0;}
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
            assert(manager != NULL);

            auto &inputsStore = manager->cm.getStore<Inputs>();
            auto &transformStore = manager->cm.getStore<Transform>();
            auto &weaponStore = manager->cm.getStore<Weapon>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Weapon::id));
                assert(manager->cm.entityHasComponent(e, Inputs::id));
                assert(manager->cm.entityHasComponent(e, Transform::id));

                auto a = inputsStore.getComponent(e);
                auto b = weaponStore.getComponent(e);

                b->timeLeft -= dt;

                if(b->timeLeft < 0.0)
                {
                    b->timeLeft = 0.0;
                }

                if(a->use == true && b->timeLeft <= 0.0)
                {
                    b->timeLeft = 0.1;

                    Entity newEntity = manager->em.getEntity();
                    if(newEntity != invalidEntity)
                    {
                        auto transform = transformStore.getComponent(e);

                        float x = transform->x + 25*cos(transform->rotation);
                        float y = transform->y + 25*sin(transform->rotation);

                        if(a->selected == 0)
                        {
                            // Bullet
                            manager->addEntityComponent<Transform>(newEntity, Transform(x, y, transform->rotation));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(200.0, transform->rotation));
                            manager->addEntityComponent<Render>(newEntity, Render(0,255,0));
                            manager->addEntityComponent<Size>(newEntity, Size(1.0));
                            manager->addEntityComponent<Timer>(newEntity, Timer(1.0));
                            manager->addEntityComponent<Projectile>(newEntity, Projectile(1));
                            manager->addEntityComponent<Collision>(newEntity, Collision(2, true));
                            manager->addEntityComponent<Health>(newEntity, Health());
                        }
                        else if(a->selected == 1)
                        {
                            // Rocket
                            manager->addEntityComponent<Transform>(newEntity, Transform(x, y, transform->rotation));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(50.0, transform->rotation));
                            manager->addEntityComponent<Render>(newEntity, Render(255,0,0));
                            manager->addEntityComponent<Size>(newEntity, Size(2.0));
                            manager->addEntityComponent<Timer>(newEntity, Timer(2.0));
                            manager->addEntityComponent<Rocket>(newEntity, Rocket(2, 0.5));
                            manager->addEntityComponent<Collision>(newEntity, Collision(2, true));
                            manager->addEntityComponent<Health>(newEntity, Health());
                            manager->addEntityComponent<Explode>(newEntity, Explode());
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
            assert(manager != NULL);

            auto &rocketStore = manager->cm.getStore<Rocket>();
            auto &transformStore = manager->cm.getStore<Transform>();
            auto &velocityStore = manager->cm.getStore<Velocity>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Rocket::id));
                assert(manager->cm.entityHasComponent(e, Transform::id));
                assert(manager->cm.entityHasComponent(e, Velocity::id));

                auto r = rocketStore.getComponent(e);

                r->boostTimeLeft -= dt;

                if(r->boostTimeLeft <= 0.0 && r->boostTimeLeft + dt > 0.0)
                {
                    auto v = velocityStore.getComponent(e);
                    v->speed *= 5;
                    manager->addEntityComponent<Trail>(e, Trail());
                }

                if(manager->cm.entityHasComponent(e, Trail::id))
                {
                    if(rand()%2 == 0)
                    {
                        Entity newEntity = manager->em.getEntity();
                        if(newEntity != invalidEntity)
                        {
                            auto transform = transformStore.getComponent(e);

                            manager->addEntityComponent<Transform>(newEntity, Transform(
                                transform->x + RAND_BETWEEN(-3.0, 3.0),
                                transform->y + RAND_BETWEEN(-3.0, 3.0),
                                RAND_BETWEEN(0, 2 * 3.142))
                            );
                            manager->addEntityComponent<Size>(newEntity, Size(1.0));
                            int n = rand()%4;
                            if(n == 0)
                            {
                                manager->addEntityComponent<Render>(newEntity, Render(220, 20, 20));
                            }
                            else if(n == 1)
                            {
                                manager->addEntityComponent<Render>(newEntity, Render(220, 200, 20));
                            }
                            else if(n == 2)
                            {
                                manager->addEntityComponent<Render>(newEntity, Render(220, 70, 20));
                            }
                            else
                            {
                                manager->addEntityComponent<Render>(newEntity, Render(50, 20, 20));
                            }
                            manager->addEntityComponent<Timer>(newEntity, Timer(RAND_BETWEEN(0.5, 0.75)));
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
            assert(manager != NULL);

            auto &timerStore = manager->cm.getStore<Timer>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Timer::id));

                auto a = timerStore.getComponent(e);

                a->timeLeft -= dt;

                if(a->timeLeft <= 0.0)
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
            assert(manager != NULL);

            auto &collisionStore = manager->cm.getStore<Collision>();
            auto &transformStore = manager->cm.getStore<Transform>();
            auto &sizeStore = manager->cm.getStore<Size>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Collision::id));
                assert(manager->cm.entityHasComponent(e, Transform::id));
                assert(manager->cm.entityHasComponent(e, Size::id));

                auto c = collisionStore.getComponent(e);
                auto a = transformStore.getComponent(e);
                auto r = sizeStore.getComponent(e);

                c->collided = false;

                for(auto e2 : entities)
                {
                    if(e == e2) {continue;} // FIXME: Try e <= e2

                    auto c2 = collisionStore.getComponent(e2);

                    // FIXME: Test this
                    if(c->mask == c2->mask && c->self == false) {continue;}
                    //if((c->mask & c2->mask) == 0) {continue;}

                    auto a2 = transformStore.getComponent(e2);
                    auto r2 = sizeStore.getComponent(e2);

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
            assert(manager != NULL);

            auto &healthStore = manager->cm.getStore<Health>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Health::id));

                auto h = healthStore.getComponent(e);

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
            assert(manager != NULL);

            auto &collisionStore = manager->cm.getStore<Collision>();
            auto &healthStore = manager->cm.getStore<Health>();
            auto &transformStore = manager->cm.getStore<Transform>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Collision::id));
                assert(manager->cm.entityHasComponent(e, Health::id));
                assert(manager->cm.entityHasComponent(e, Transform::id));

                auto c = collisionStore.getComponent(e);
                auto h = healthStore.getComponent(e);

                if(c->collided == true && h->immunity <= 0.0)
                {
                    h->health--;
                    h->immunity = 0.1;

                    if(h->health <= 0)
                    {
                        manager->remove.push_back(e);

                        if(manager->cm.entityHasComponent(e, Explode::id))
                        {
                            for(int i = 0; i < 20; ++i)
                            {
                                Entity newEntity = manager->em.getEntity();
                                if(newEntity != invalidEntity)
                                {
                                    auto transform = transformStore.getComponent(e);

                                    manager->addEntityComponent<Transform>(newEntity, Transform(
                                        transform->x + RAND_BETWEEN(-4.0, 4.0)*RAND_BETWEEN(-4.0, 4.0),
                                        transform->y + RAND_BETWEEN(-4.0, 4.0)*RAND_BETWEEN(-4.0, 4.0),
                                        RAND_BETWEEN(0, 2 * 3.142))
                                    );
                                    manager->addEntityComponent<Size>(newEntity, Size(5.0));
                                    int n = rand()%4;
                                    if(n == 0)
                                    {
                                        manager->addEntityComponent<Render>(newEntity, Render(220, 20, 20));
                                    }
                                    else if(n == 1)
                                    {
                                        manager->addEntityComponent<Render>(newEntity, Render(220, 200, 20));
                                    }
                                    else if(n == 2)
                                    {
                                        manager->addEntityComponent<Render>(newEntity, Render(220, 70, 20));
                                    }
                                    else
                                    {
                                        manager->addEntityComponent<Render>(newEntity, Render(50, 20, 20));
                                    }
                                    manager->addEntityComponent<Timer>(newEntity, Timer(RAND_BETWEEN(0.1, 1.0)));
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
            assert(manager != NULL);

            auto &transformStore = manager->cm.getStore<Transform>();
            auto &sizeStore = manager->cm.getStore<Size>();
            auto &healthStore = manager->cm.getStore<Health>();

            for(auto e : entities)
            {
                assert(manager->cm.entityHasComponent(e, Transform::id));
                assert(manager->cm.entityHasComponent(e, Size::id));
                assert(manager->cm.entityHasComponent(e, Health::id));

                auto health = healthStore.getComponent(e);
                auto size = sizeStore.getComponent(e);

                if(health->health <= 0 && size->radius >= 6.0)
                {
                    auto transform = transformStore.getComponent(e);

                    // New asteroids
                    for(int i = 0; i < rand()%2+3; ++i)
                    {
                        Entity newEntity = manager->em.getEntity();
                        if(newEntity != invalidEntity)
                        {
                            int colour = RAND_BETWEEN(100, 200);
                            manager->addEntityComponent<Transform>(newEntity, Transform(transform->x, transform->y, RAND_BETWEEN(0, 2 * 3.142)));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(RAND_BETWEEN(50.0, 100.0), RAND_BETWEEN(0, 2 * 3.142)));
                            manager->addEntityComponent<Size>(newEntity, Size(size->radius/2));
                            manager->addEntityComponent<Render>(newEntity, Render(colour, colour, colour));
                            manager->addEntityComponent<Collision>(newEntity, Collision(3, false));
                            manager->addEntityComponent<Health>(newEntity, Health(health->startHealth - 1));
                            manager->addEntityComponent<Asteroid>(newEntity, Asteroid());
                            manager->addEntityComponent<Trail>(newEntity, Trail());
                        }
                    }

                    // Pretty particles
                    for(int i = 0; i < rand()%5+20; ++i)
                    {
                        Entity newEntity = manager->em.getEntity();
                        if(newEntity != invalidEntity)
                        {
                            manager->addEntityComponent<Transform>(newEntity, Transform(transform->x, transform->y, RAND_BETWEEN(0, 2 * 3.142)));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(RAND_BETWEEN(150.0, 300.0), RAND_BETWEEN(0, 2 * 3.142)));
                            manager->addEntityComponent<Size>(newEntity, Size(1.0));
                            if(rand()%2 == 0)
                            {
                                manager->addEntityComponent<Render>(newEntity, Render(220, 20, 20));
                            }
                            else
                            {
                                manager->addEntityComponent<Render>(newEntity, Render(220, 140, 20));
                            }
                            manager->addEntityComponent<Timer>(newEntity, Timer(0.5));
                        }
                    }
                }
            }
        }
    private:
};


#endif