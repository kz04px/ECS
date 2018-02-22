#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "ecs.hpp"
#include <SDL.h>

class PhysicsSystem : public System
{
    public:
        PhysicsSystem(EntityManager *em, ComponentManager *cm)
        {
            this->em = em;
            this->cm = cm;
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
                a->x += dt * b->x + 0.1;
                a->y += dt * b->y + 0.2;
            }
        }
    private:
        EntityManager *em;
        ComponentManager *cm;
};


class DamageSystem : public System
{
    public:
        DamageSystem(EntityManager *em, ComponentManager *cm)
        {
            this->em = em;
            this->cm = cm;
            required.insert(Position::id);
            required.insert(Velocity::id);
        }
        void update(const float dt)
        {
            assert(em != NULL);
            assert(cm != NULL);
        }
    private:
        EntityManager *em;
        ComponentManager *cm;
};


class RenderSystem : public System
{
    public:
        RenderSystem(EntityManager *em, ComponentManager *cm)
        {
            this->em = em;
            this->cm = cm;
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
                r.x = a->x;
                r.y = a->y;
                r.w = 5;
                r.h = 5;
                SDL_RenderFillRect(renderer, &r);
            }
        }
        SDL_Renderer *renderer;
    private:
        EntityManager *em;
        ComponentManager *cm;
};

#endif