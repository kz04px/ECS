#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <cmath>
#include "ecs.hpp"

#define RAND_BETWEEN(a, b) ((double)rand()/RAND_MAX * (b-a) + a)

class Transform
{
    public:
        Transform() : x(0.0), y(0.0), rotation(0.0)
        {
        }
        Transform(float x, float y, float radians) : x(x), y(y), rotation(radians)
        {
        }
        static const Component id;
        float x;
        float y;
        float rotation;
    private:
};

class Velocity
{
    public:
        Velocity() : x(0.0), y(0.0)
        {
        }
        Velocity(float s, float dir) : x(s*cos(dir)), y(s*sin(dir))
        {
        }
        static const Component id;
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
        Render() : red(255), green(0), blue(0), alpha(255), texture(0)
        {
        }
        Render(int texture) : red(255), green(0), blue(0), alpha(255), texture(texture)
        {
        }
        Render(int r, int g, int b) : red(r), green(g), blue(b), alpha(255), texture(0)
        {
        }
        static const Component id;
        int red;
        int green;
        int blue;
        int alpha;
        int texture;
    private:
};

class Inputs
{
    public:
        Inputs() : left(false), right(false), up(false), down(false), use(false), mouse_x(0.0), mouse_y(0.0), selected(0)
        {
        }
        static const Component id;
        bool left;
        bool right;
        bool up;
        bool down;
        bool use;
        int mouse_x;
        int mouse_y;
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
        Weapon() : time_left(0.0)
        {
        }
        static const Component id;
        float time_left;
    private:
};

class Timer
{
    public:
        Timer() : time_left(3.0)
        {
        }
        explicit Timer(float time_left) : time_left(time_left)
        {
        }
        static const Component id;
        float time_left;
    private:
};

class Projectile
{
    public:
        Projectile() : owner(invalid_entity), damage(1)
        {
        }
        Projectile(Entity e, int damage) : owner(e), damage(damage)
        {
        }
        static const Component id;
        Entity owner;
        int damage;
    private:
};

class Rocket
{
    public:
        Rocket() : owner(invalid_entity), damage(1), boost_time_left(1.0)
        {
        }
        Rocket(Entity e, int d, float t) : owner(e), damage(d), boost_time_left(t)
        {
        }
        static const Component id;
        Entity owner;
        int damage;
        float boost_time_left;
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
        Health() : start_health(1), health(1), immunity(0.0)
        {
        }
        explicit Health(int health) : start_health(health), health(health), immunity(0.0)
        {
        }
        static const Component id;
        int start_health;
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

class Trail
{
    public:
        Trail()
        {
        }
        Trail(int r, int g, int b) : red(r), green(g), blue(b)
        {
        }
        static const Component id;
        int red;
        int green;
        int blue;
    private:
};

class Explode
{
    public:
        Explode()
        {
        }
        Explode(int r, int d) : radius(r), damage(d)
        {
        }
        static const Component id;
        int radius;
        int damage;
    private:
};

class Fade
{
    public:
        Fade() : fade_time(1.0), time(0.0)
        {
        }
        Fade(float t) : fade_time(t), time(0.0)
        {
        }
        static const Component id;
        float fade_time;
        float time;
    private:
};

class Player
{
    public:
        Player()
        {
        }
        static const Component id;
    private:
};

class AI
{
    public:
        AI() : aggressive(true), timer(0.0)
        {
        }
        static const Component id;
        bool aggressive;
        float timer;
    private:
};

class MineAI
{
    public:
        MineAI() : aggressive(true), timer(0.0)
        {
        }
        static const Component id;
        bool aggressive;
        float timer;
    private:
};

class Ship
{
    public:
        Ship()
        {
        }
        static const Component id;
    private:
};

const Component Transform::id = 0;
const Component Velocity::id = 1;
const Component Size::id = 2;
const Component Render::id = 3;
const Component Inputs::id = 4;
const Component Weapon::id = 5;
const Component Timer::id = 6;
const Component Projectile::id = 7;
const Component Collision::id = 8;
const Component Health::id = 9;
const Component Asteroid::id = 10;
const Component Rocket::id = 11;
const Component Trail::id = 12;
const Component Explode::id = 13;
const Component Fade::id = 14;
const Component Player::id = 15;
const Component AI::id = 16;
const Component MineAI::id = 17;
const Component Ship::id = 18;

#endif
