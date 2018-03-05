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


class CollisionSystem : public System
{
    public:
        CollisionSystem()
        {
            required.insert(Position::id);
            required.insert(Collision::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &positionStore  = manager->cm.getStore<Position>();
            auto &collisionStore = manager->cm.getStore<Collision>();
            auto &sizeStore      = manager->cm.getStore<Size>();

            for(auto e : entities)
            {
                auto a = positionStore.getComponent(e);
                auto b = collisionStore.getComponent(e);
                auto c = sizeStore.getComponent(e);
                b->collision = false;

                for(auto e2 : manager->em.allEntities)
                {
                    if(e == e2) {continue;}

                    auto a2 = positionStore.getComponent(e2);
                    auto c2 = sizeStore.getComponent(e2);
                    float dist = c->radius + c2->radius;

                    if(abs(a->x - a2->x) <= dist && abs(a->y - a2->y) <= dist)
                    {
                        b->collision = true;

                        if(c2->radius >= 2.0)
                        {
                            // Add new asteroids
                            for(int i = 0; i < rand()%2+2; ++i)
                            {
                                Entity newEntity = manager->em.getEntity();
                                if(newEntity != invalidEntity)
                                {
                                    float colour = RAND_BETWEEN(100, 200);
                                    manager->addEntityComponent<Position>(newEntity, Position(a2->x, a2->y));
                                    manager->addEntityComponent<Velocity>(newEntity, Velocity(RAND_BETWEEN(5.0, 10.0), RAND_BETWEEN(0, 2 * 3.142)));
                                    manager->addEntityComponent<Render>(newEntity, Render(colour, colour, colour));
                                    manager->addEntityComponent<Size>(newEntity, Size(c2->radius/2));
                                }
                            }
                        }

                        manager->em.removeEntity(e2);
                        manager->cm.removeEntity(e2);
                        manager->sm.removeEntity(e2);
                        break;
                    }
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
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &h = manager->cm.getStore<Health>();
            auto &c = manager->cm.getStore<Collision>();

            for(auto e : entities)
            {
                auto a = h.getComponent(e);
                auto b = c.getComponent(e);

                a->timeSinceDamaged += dt;
                a->timeAlive += dt;

                // Can only be damaged so often
                if(a->timeSinceDamaged > 1.0 && b->collision == true)
                {
                    a->health--;
                    a->timeSinceDamaged = 0.0;
                }

                if(a->health <= 0)
                {
                    a->dead = true;
                }
            }
        }
    private:
};


class RenderSystem : public System
{
    public:
        explicit RenderSystem(SDL_Renderer *r) : renderer(r)
        {
            required.insert(Position::id);
            required.insert(Render::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);
            assert(renderer != NULL);

            auto &p = manager->cm.getStore<Position>();
            auto &s = manager->cm.getStore<Size>();
            auto &r = manager->cm.getStore<Render>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                auto b = s.getComponent(e);
                auto c = r.getComponent(e);

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
                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }
        }
    private:
        SDL_Renderer *renderer;
};


class InputSystem : public System
{
    public:
        InputSystem()
        {
            required.insert(Position::id);
            required.insert(Velocity::id);
            required.insert(Inputs::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &velocityStore = manager->cm.getStore<Velocity>();
            auto &inputsStore   = manager->cm.getStore<Inputs>();

            for(auto e : entities)
            {
                auto a = velocityStore.getComponent(e);
                auto b = inputsStore.getComponent(e);

                if(b->left == true)       {a->x = -1.0;}
                else if(b->right == true) {a->x =  1.0;}
                else                      {a->x =  0.0;}
                if(b->up == true)         {a->y = -1.0;}
                else if(b->down == true)  {a->y =  1.0;}
                else                      {a->y =  0.0;}
            }
        }
    private:
};


class DeathSystem : public System
{
    public:
        DeathSystem()
        {
            required.insert(Health::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &h = manager->cm.getStore<Health>();

            for(auto e : entities)
            {
                auto a = h.getComponent(e);

                if(a->dead == true)
                {
                    std::cout << "Died after " << (int)a->timeAlive << " second" << ((int)a->timeAlive == 1 ? "" : "s") << "." << std::endl;
                    a->dead = false;
                    a->health = 1;
                    a->timeAlive = 0.0;
                }
            }
        }
    private:
};


class ItemSystem : public System
{
    public:
        ItemSystem()
        {
            required.insert(Position::id);
            required.insert(Inventory::id);
            required.insert(Inputs::id);
        }
        void update(const float dt)
        {
            assert(manager != NULL);

            auto &positionStore  = manager->cm.getStore<Position>();
            auto &inventoryStore = manager->cm.getStore<Inventory>();
            auto &inputsStore    = manager->cm.getStore<Inputs>();
            auto &useStore       = manager->cm.getStore<Use>();

            for(auto e : entities)
            {
                auto a = inputsStore.getComponent(e);
                auto b = positionStore.getComponent(e);
                auto c = inventoryStore.getComponent(e);

                if(a->selected >= c->items.size())
                {
                    continue;
                }

                for(auto i : c->items)
                {
                    auto d = useStore.getComponent(i);

                    if(d->uses <= 0)
                    {

                    }

                    d->lastUsed -= dt;
                    if(d->lastUsed < 0.0)
                    {
                        d->lastUsed = 0.0;
                    }
                }

                auto d = useStore.getComponent(c->items[a->selected]);

                if(a->use == true && d->uses != 0 && d->lastUsed <= 0.0)
                {
                    d->lastUsed = d->cooldown;

                    if(d->uses > 0)
                    {
                        d->uses--;
                    }

                    Entity newEntity = manager->em.getEntity();
                    if(newEntity != invalidEntity)
                    {
                        auto pos = positionStore.getComponent(e);

                        float dx = a->mouseX - pos->x;
                        float dy = a->mouseY - pos->y;
                        float dir = atan2(dy, dx);

                        float x = b->x + 15*cos(dir);
                        float y = b->y + 15*sin(dir);

                        if(a->selected == 0)
                        {
                            // Laser
                            manager->addEntityComponent<Position>(newEntity, Position(x, y));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(20.0, dir));
                            manager->addEntityComponent<Render>(newEntity, Render(0,255,0));
                            manager->addEntityComponent<Size>(newEntity, Size(1.0));
                            manager->addEntityComponent<Collision>(newEntity, Collision());
                            manager->addEntityComponent<Health>(newEntity, Health());
                        }
                        else if(a->selected == 1)
                        {
                            // Rocket
                            manager->addEntityComponent<Position>(newEntity, Position(x, y));
                            manager->addEntityComponent<Velocity>(newEntity, Velocity(5.0, dir));
                            manager->addEntityComponent<Render>(newEntity, Render(255,0,0));
                            manager->addEntityComponent<Size>(newEntity, Size(2.0));
                            manager->addEntityComponent<Collision>(newEntity, Collision());
                            manager->addEntityComponent<Health>(newEntity, Health());
                        }
                    }
                }
            }
        }
    private:
};


#endif