#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <cmath>
#include "ecs.hpp"


#define RAND_BETWEEN(a, b) ((double)rand()/RAND_MAX * (b-a) + a)


class Position
{
    public:
        Position() : x(0.0), y(0.0) {}
        Position(float a, float b) : x(a), y(b)
        {
        }
        static const Component id;
        float x;
        float y;
    private:
};


class Velocity
{
    public:
        Velocity() : speed(0.0), x(0.0), y(1.0) {}
        explicit Velocity(float s, float dir) : speed(s), x(cos(dir)), y(sin(dir))
        {
        }
        static const Component id;
        float speed;
        float x;
        float y;
    private:
};


class Health
{
    public:
        Health() : health(1), timeSinceDamaged(0.0), timeAlive(0.0), dead(false)
        {
        }
        static const Component id;
        int health;
        float timeSinceDamaged;
        float timeAlive;
        bool dead;
    private:
};


class Render
{
    public:
        Render() : red(255), green(0), blue(0) {}
        Render(int r, int g, int b) : red(r), green(g), blue(b)
        {
        }
        static const Component id;
        int red;
        int green;
        int blue;
    private:
};


class Inputs
{
    public:
        Inputs() : left(false), right(false), up(false), down(false), use(false), mouseX(0.0), mouseY(0.0)
        {
        }
        static const Component id;
        bool left;
        bool right;
        bool up;
        bool down;
        bool use;
        int mouseX;
        int mouseY;
    private:
};


class Size
{
    public:
        Size() : radius(1.0) {}
        explicit Size(float r) : radius(r)
        {
        }
        static const Component id;
        float radius;
    private:
};


class Collision
{
    public:
        Collision() : collision(false)
        {
        }
        static const Component id;
        bool collision;
    private:
};


class Inventory
{
    public:
        Inventory() : items({})
        {
        }
        static const Component id;
        std::vector<Entity> items;
    private:
};


class Use
{
    public:
        Use() : uses(-1), lastUsed(0.0), cooldown(1.0)
        {
        }
        static const Component id;
        int uses;
        float lastUsed;
        float cooldown;
    private:
};


const Component Position::id = 0;
const Component Velocity::id = 1;
const Component Health::id = 2;
const Component Render::id = 3;
const Component Inputs::id = 4;
const Component Size::id = 5;
const Component Collision::id = 6;
const Component Inventory::id = 7;
const Component Use::id = 8;


#endif