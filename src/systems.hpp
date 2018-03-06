#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP


#include "ecs.hpp"
#include <SDL.h>


class MovementSystem : public System
{
    public:
        MovementSystem()
        {
            required.insert(Position::id);
            required.insert(Velocity::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &p = manager->cm.getStore<Position>();
            auto &v = manager->cm.getStore<Velocity>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                auto b = v.getComponent(e);

                a->x += dt * b->speed * b->x;
                a->y += dt * b->speed * b->y;

                if(a->x > 512) {a->x -= 512;}
                if(a->x <   0) {a->x += 512;}

                if(a->y > 512) {a->y -= 512;}
                if(a->y <   0) {a->y += 512;}
            }
        }
    private:
};


class RenderSystem : public System
{
    public:
        RenderSystem(SDL_Renderer *r, SDL_Texture *shipTexture) : renderer(r), shipTexture(shipTexture)
        {
            required.insert(Position::id);
            required.insert(Render::id);
            required.insert(Size::id);
            required.insert(Rotation::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);
            assert(renderer != NULL);

            auto &p = manager->cm.getStore<Position>();
            auto &s = manager->cm.getStore<Size>();
            auto &r = manager->cm.getStore<Render>();
            auto &rotation = manager->cm.getStore<Rotation>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                auto b = s.getComponent(e);
                auto c = r.getComponent(e);
                auto d = rotation.getComponent(e);

                SDL_SetRenderDrawColor(renderer, c->red, c->green, c->blue, 255);

                for(int x = -1; x < 2; ++x)
                {
                    for(int y = -1; y < 2; ++y)
                    {
                        SDL_Rect rect;
                        rect.x = a->x - b->radius + x*512;
                        rect.y = a->y - b->radius + y*512;
                        rect.w = 2 * b->radius;
                        rect.h = 2 * b->radius;

                        if(c->texture == 1)
                        {
                            SDL_Point center = {b->radius, b->radius};
                            SDL_RenderCopyEx(renderer, shipTexture, NULL, &rect, d->radians*180/3.142 - 90, &center, SDL_FLIP_NONE);
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
            required.insert(Position::id);
            required.insert(Velocity::id);
            required.insert(Rotation::id);
            required.insert(Inputs::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &positionStore = manager->cm.getStore<Position>();
            auto &velocityStore = manager->cm.getStore<Velocity>();
            auto &inputsStore   = manager->cm.getStore<Inputs>();
            auto &rotationStore = manager->cm.getStore<Rotation>();

            for(auto e : entities)
            {
                auto a = velocityStore.getComponent(e);
                auto b = inputsStore.getComponent(e);
                auto c = rotationStore.getComponent(e);
                auto d = positionStore.getComponent(e);

                float dx = b->mouseX - d->x;
                float dy = b->mouseY - d->y;
                c->radians = atan2(-dy, -dx);

                if(b->left == true)       {a->x -= 0.05;}
                else if(b->right == true) {a->x += 0.05;}
                if(b->up == true)         {a->y -= 0.05;}
                else if(b->down == true)  {a->y += 0.05;}

                if(a->x > 5.0)       {a->x =  5.0;}
                else if(a->x < -5.0) {a->x = -5.0;}
                if(a->y > 5.0)       {a->y =  5.0;}
                else if(a->y < -5.0) {a->y = -5.0;}
            }
        }
    private:
};


class RemoveSystem : public System
{
    public:
        RemoveSystem()
        {
            required.insert(Remove::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &removeStore = manager->cm.getStore<Remove>();

            for(auto e : entities)
            {
                auto a = removeStore.getComponent(e);

                if(a->remove == true)
                {
                    manager->em.removeEntity(e);
                    manager->cm.removeEntity(e);
                    manager->sm.removeEntity(e);
                }
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
            required.insert(Position::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &inputsStore = manager->cm.getStore<Inputs>();
            auto &positionStore = manager->cm.getStore<Position>();
            auto &weaponStore = manager->cm.getStore<Weapon>();

            for(auto e : entities)
            {
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
                        auto pos = positionStore.getComponent(e);

                        float dx = a->mouseX - pos->x;
                        float dy = a->mouseY - pos->y;
                        float dir = atan2(dy, dx);

                        float x = pos->x + 25*cos(dir);
                        float y = pos->y + 25*sin(dir);

                        if(a->selected == 0)
                        {
                            // Laser
                            manager->addEntityComponent<Position>(newEntity, Position(x, y));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(200.0, dir));
                            manager->addEntityComponent<Render>(newEntity, Render(0,255,0));
                            manager->addEntityComponent<Size>(newEntity, Size(1.0));
                            manager->addEntityComponent<Timer>(newEntity, Timer(1.0));
                            manager->addEntityComponent<Remove>(newEntity, Remove());
                            manager->addEntityComponent<Projectile>(newEntity, Projectile(0));
                            manager->addEntityComponent<Collision>(newEntity, Collision(2, true));
                            manager->addEntityComponent<Health>(newEntity, Health());
                            manager->addEntityComponent<Rotation>(newEntity, Rotation());
                        }
                        else if(a->selected == 1)
                        {
                            // Rocket
                            manager->addEntityComponent<Position>(newEntity, Position(x, y));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(50.0, dir));
                            manager->addEntityComponent<Render>(newEntity, Render(255,0,0));
                            manager->addEntityComponent<Size>(newEntity, Size(2.0));
                            manager->addEntityComponent<Timer>(newEntity, Timer(2.0));
                            manager->addEntityComponent<Remove>(newEntity, Remove());
                            manager->addEntityComponent<Projectile>(newEntity, Projectile(1));
                            manager->addEntityComponent<Collision>(newEntity, Collision(2, true));
                            manager->addEntityComponent<Health>(newEntity, Health());
                            manager->addEntityComponent<Rotation>(newEntity, Rotation());
                        }
                    }
                }
            }
        }
    private:
};


class ProjectileSystem : public System
{
    public:
        ProjectileSystem()
        {
            required.insert(Projectile::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &projectileStore = manager->cm.getStore<Projectile>();

            for(auto e : entities)
            {
                auto a = projectileStore.getComponent(e);
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
            required.insert(Remove::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &timerStore = manager->cm.getStore<Timer>();
            auto &removeStore = manager->cm.getStore<Remove>();

            for(auto e : entities)
            {
                auto a = timerStore.getComponent(e);
                auto b = removeStore.getComponent(e);

                a->timeLeft -= dt;

                if(a->timeLeft <= 0.0)
                {
                    b->remove = true;
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
            required.insert(Position::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &collisionStore = manager->cm.getStore<Collision>();
            auto &positionStore = manager->cm.getStore<Position>();
            auto &sizeStore = manager->cm.getStore<Size>();

            for(auto e : entities)
            {
                auto c = collisionStore.getComponent(e);
                auto a = positionStore.getComponent(e);
                auto r = sizeStore.getComponent(e);

                c->collided = false;

                for(auto e2 : entities)
                {
                    if(e == e2) {continue;} // FIXME: Try e <= e2

                    auto c2 = collisionStore.getComponent(e2);

                    // FIXME: Test this
                    if(c->mask == c2->mask && c->self == false) {continue;}
                    //if((c->mask & c2->mask) == 0) {continue;}

                    auto a2 = positionStore.getComponent(e2);
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
            required.insert(Remove::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &collisionStore = manager->cm.getStore<Collision>();
            auto &healthStore = manager->cm.getStore<Health>();
            auto &removeStore = manager->cm.getStore<Remove>();

            for(auto e : entities)
            {
                auto c = collisionStore.getComponent(e);
                auto h = healthStore.getComponent(e);

                if(c->collided == true && h->immunity <= 0.0)
                {
                    h->health--;
                    h->immunity = 0.1;

                    if(h->health <= 0)
                    {
                        auto r = removeStore.getComponent(e);
                        r->remove = true;
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
            required.insert(Position::id);
            required.insert(Velocity::id);
            required.insert(Size::id);
            required.insert(Render::id);
            required.insert(Collision::id);
            required.insert(Health::id);
            required.insert(Remove::id);
            required.insert(Asteroid::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &positionStore = manager->cm.getStore<Position>();
            auto &velocityStore = manager->cm.getStore<Velocity>();
            auto &sizeStore = manager->cm.getStore<Size>();
            auto &renderStore = manager->cm.getStore<Render>();
            auto &healthStore = manager->cm.getStore<Health>();
            auto &removeStore = manager->cm.getStore<Remove>();

            for(auto e : entities)
            {
                auto remove = removeStore.getComponent(e);
                auto size = sizeStore.getComponent(e);

                if(remove->remove == true && size->radius >= 6.0)
                {
                    int colour = renderStore.getComponent(e)->red;
                    auto pos = positionStore.getComponent(e);
                    auto health = healthStore.getComponent(e);

                    // New asteroids
                    for(int i = 0; i < rand()%2+3; ++i)
                    {
                        Entity newEntity = manager->em.getEntity();
                        if(newEntity != invalidEntity)
                        {
                            manager->addEntityComponent<Position>(newEntity, Position(pos->x, pos->y));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(RAND_BETWEEN(50.0, 100.0), RAND_BETWEEN(0, 2 * 3.142)));
                            manager->addEntityComponent<Size>(newEntity, Size(size->radius/2));
                            manager->addEntityComponent<Render>(newEntity, Render(colour, colour, colour));
                            manager->addEntityComponent<Collision>(newEntity, Collision(3, false));
                            manager->addEntityComponent<Health>(newEntity, Health(health->startHealth - 1));
                            manager->addEntityComponent<Remove>(newEntity, Remove());
                            manager->addEntityComponent<Asteroid>(newEntity, Asteroid());
                            manager->addEntityComponent<Rotation>(newEntity, Rotation());
                        }
                    }

                    // Pretty particles
                    for(int i = 0; i < rand()%5+20; ++i)
                    {
                        Entity newEntity = manager->em.getEntity();
                        if(newEntity != invalidEntity)
                        {
                            manager->addEntityComponent<Position>(newEntity, Position(pos->x, pos->y));
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
                            manager->addEntityComponent<Remove>(newEntity, Remove());
                            manager->addEntityComponent<Rotation>(newEntity, Rotation());
                        }
                    }
                }
            }
        }
    private:
};


#endif