#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <cmath>
#include "ecs.hpp"


#define RAND_BETWEEN(a, b) ((double)rand()/RAND_MAX * (b-a) + a)


class Position
{
    public:
        Position() {}
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
        Velocity() {}
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
        Render() {}
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
        Inputs() : left(false), right(false), up(false), down(false)
        {
        }
        static const Component id;
        bool left;
        bool right;
        bool up;
        bool down;
    private:
};


class Size
{
    public:
        Size() {}
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


const Component Position::id = 0;
const Component Velocity::id = 1;
const Component Health::id = 2;
const Component Render::id = 3;
const Component Inputs::id = 4;
const Component Size::id = 5;
const Component Collision::id = 6;


#endif