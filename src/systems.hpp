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
            required.insert(Health::id);
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

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            auto &p = cm->getStore<Position>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                SDL_Rect r;
                r.x = a->x - radius;
                r.y = a->y - radius;
                r.w = 2 * radius;
                r.h = 2 * radius;
                SDL_RenderFillRect(renderer, &r);
            }
        }
        EntityManager *em;
        ComponentManager *cm;
    private:
        const float radius = 2.5;
        SDL_Renderer *renderer;
};


class RenderPlayerSystem : public System
{
    public:
        RenderPlayerSystem(SDL_Renderer *r)
        {
            this->renderer = r;
            required.insert(Position::id);
            required.insert(Render::id);
            required.insert(Inputs::id);
        }
        void update(const float dt)
        {
            assert(em != NULL);
            assert(cm != NULL);
            assert(renderer != NULL);

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

            auto &p = cm->getStore<Position>();

            for(auto e : entities)
            {
                auto a = p.getComponent(e);
                SDL_Rect r = {a->x - radius, a->y - radius, 2 * radius, 2 * radius};
                SDL_RenderFillRect(renderer, &r);
            }
        }
        EntityManager *em;
        ComponentManager *cm;
    private:
        const float radius = 5;
        SDL_Renderer *renderer;
};


class MovementSystem : public System
{
    public:
        MovementSystem()
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

#endif