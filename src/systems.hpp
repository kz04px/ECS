#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "ecs.hpp"
#include <SDL.h>

class PhysicsSystem : public System
{
    public:
        PhysicsSystem()
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

                a->x += dt * b->x;
                a->y += dt * b->y;

                if(a->x > 512) {b->x *= -1;}
                if(a->x <   0) {b->x *= -1;}

                if(a->y > 512) {b->y *= -1;}
                if(a->y <   0) {b->y *= -1;}
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
            required.insert(Position::id);
            required.insert(Velocity::id);
        }
        void update(const float dt)
        {
            assert(em != NULL);
            assert(cm != NULL);
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
        }
        void update(const float dt)
        {
            assert(em != NULL);
            assert(cm != NULL);
            assert(renderer != NULL);

            auto &p = cm->getStore<Position>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                SDL_Rect r;
                r.x = a->x - 2.5;
                r.y = a->y - 2.5;
                r.w = 5;
                r.h = 5;
                SDL_RenderFillRect(renderer, &r);
            }
        }
        EntityManager *em;
        ComponentManager *cm;
    private:
        SDL_Renderer *renderer;
};

#endif