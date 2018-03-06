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


class Rotation
{
    public:
        Rotation() : radians(0.0)
        {
        }
        explicit Rotation(float radians) : radians(radians)
        {
        }
        static const Component id;
        float radians;
    private:
};


class Velocity
{
    public:
        Velocity() : speed(0.0), x(0.0), y(1.0)
        {
        }
        explicit Velocity(float s) : speed(s), x(0.0), y(0.0)
        {
        }
        Velocity(float s, float dir) : speed(s), x(cos(dir)), y(sin(dir))
        {
        }
        static const Component id;
        float speed;
        float x;
        float y;
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


class Render
{
    public:
        Render() : red(255), green(0), blue(0), texture(0)
        {
        }
        Render(int texture) : red(255), green(0), blue(0), texture(texture)
        {
        }
        Render(int r, int g, int b) : red(r), green(g), blue(b), texture(0)
        {
        }
        static const Component id;
        int red;
        int green;
        int blue;
        int texture;
    private:
};


class Inputs
{
    public:
        Inputs() : left(false), right(false), up(false), down(false), use(false), mouseX(0.0), mouseY(0.0), selected(0)
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
        int selected;
    private:
};


class Remove
{
    public:
        Remove() : remove(false)
        {
        }
        static const Component id;
        bool remove;
    private:
};


class Weapon
{
    public:
        Weapon() : timeLeft(0.0)
        {
        }
        static const Component id;
        float timeLeft;
    private:
};


class Timer
{
    public:
        Timer() : timeLeft(3.0)
        {
        }
        explicit Timer(float timeLeft) : timeLeft(timeLeft)
        {
        }
        static const Component id;
        float timeLeft;
    private:
};


class Projectile
{
    public:
        Projectile() : type(0)
        {
        }
        explicit Projectile(int type) : type(type)
        {
        }
        static const Component id;
        int type;
    private:
};


class Collision
{
    public:
        Collision() : collided(false), mask(0xFF)
        {
        }
        Collision(uint8_t mask, bool self) : collided(false), mask(mask), self(self)
        {
        }
        static const Component id;
        bool collided;
        uint8_t mask;
        bool self;
    private:
};


class Health
{
    public:
        Health() : startHealth(1), health(1), immunity(0.0)
        {
        }
        explicit Health(int health) : startHealth(health), health(health), immunity(0.0)
        {
        }
        static const Component id;
        int startHealth;
        int health;
        float immunity;
    private:
};


class Asteroid
{
    public:
        Asteroid()
        {
        }
        static const Component id;
    private:
};


const Component Position::id = 0;
const Component Velocity::id = 1;
const Component Size::id = 2;
const Component Render::id = 3;
const Component Inputs::id = 4;
const Component Remove::id = 5;
const Component Weapon::id = 6;
const Component Timer::id = 7;
const Component Projectile::id = 8;
const Component Collision::id = 9;
const Component Health::id = 10;
const Component Asteroid::id = 11;
const Component Rotation::id = 12;


#endif