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
            assert(em != NULL);
            assert(cm != NULL);

            auto &p = cm->getStore<Position>();
            auto &v = cm->getStore<Velocity>();

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
        EntityManager *em;
        ComponentManager *cm;
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
            assert(em != NULL);
            assert(cm != NULL);

            auto &positionStore = cm->getStore<Position>();
            auto &collisionStore = cm->getStore<Collision>();
            auto &sizeStore = cm->getStore<Size>();

            for(auto e : entities)
            {
                auto a = positionStore.getComponent(e);
                auto b = collisionStore.getComponent(e);
                auto c = sizeStore.getComponent(e);
                b->collision = false;

                for(auto e2 : em->allEntities)
                {
                    if(e == e2) {continue;}

                    auto a2 = positionStore.getComponent(e2);
                    auto c2 = sizeStore.getComponent(e2);
                    float dist = c->radius + c2->radius;

                    if(abs(a->x - a2->x) <= dist && abs(a->y - a2->y) <= dist)
                    {
                        b->collision = true;
                        break;
                    }
                }
            }
        }
        EntityManager *em;
        ComponentManager *cm;
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
            assert(em != NULL);
            assert(cm != NULL);

            auto &h = cm->getStore<Health>();
            auto &c = cm->getStore<Collision>();

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
        EntityManager *em;
        ComponentManager *cm;
    private:
};


class RenderSystem : public System
{
    public:
        RenderSystem(SDL_Renderer *r)
        {
            this->renderer = r;
            required.insert(Position::id);
            required.insert(Render::id);
            required.insert(Size::id);
        }
        void update(const float dt)
        {
            assert(em != NULL);
            assert(cm != NULL);
            assert(renderer != NULL);

            auto &p = cm->getStore<Position>();
            auto &s = cm->getStore<Size>();
            auto &r = cm->getStore<Render>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                auto b = s.getComponent(e);
                auto c = r.getComponent(e);

                SDL_SetRenderDrawColor(renderer, c->red, c->green, c->blue, 255);

                SDL_Rect rect;
                rect.x = a->x - b->radius;
                rect.y = a->y - b->radius;
                rect.w = 2 * b->radius;
                rect.h = 2 * b->radius;
                SDL_RenderFillRect(renderer, &rect);
            }
        }
        EntityManager *em;
        ComponentManager *cm;
    private:
        SDL_Renderer *renderer;
};


class InputSystem : public System
{
    public:
        InputSystem()
        {
            required.insert(Velocity::id);
            required.insert(Inputs::id);
        }
        void update(const float dt)
        {
            assert(em != NULL);
            assert(cm != NULL);

            auto &v = cm->getStore<Velocity>();
            auto &i = cm->getStore<Inputs>();

            for(auto e : entities)
            {
                auto a = v.getComponent(e);
                auto b = i.getComponent(e);

                if(b->left == true)       {a->x = -1.0;}
                else if(b->right == true) {a->x =  1.0;}
                else                      {a->x =  0.0;}

                if(b->up == true)         {a->y = -1.0;}
                else if(b->down == true)  {a->y =  1.0;}
                else                      {a->y =  0.0;}
            }
        }
        EntityManager *em;
        ComponentManager *cm;
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
            assert(em != NULL);
            assert(cm != NULL);

            auto &h = cm->getStore<Health>();

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
        EntityManager *em;
        ComponentManager *cm;
    private:
};

#endif