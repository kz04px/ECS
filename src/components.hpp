#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "ecs.hpp"

class Position
{
    public:
        Position()
        {
            x = (double)rand()/RAND_MAX * 512;
            y = (double)rand()/RAND_MAX * 512;
        }
        static const Component id;
        float x;
        float y;
    private:
};


class Velocity
{
    public:
        Velocity()
        {
            x = (double)rand()/RAND_MAX * 3 - 1.5;
            y = (double)rand()/RAND_MAX * 3 - 1.5;
        }
        static const Component id;
        float x;
        float y;
    private:
};


class Health
{
    public:
        static const Component id;
        int health;
    private:
};


class Render
{
    public:
        static const Component id;
        int hi;
    private:
};

const Component Position::id = 0;
const Component Velocity::id = 1;
const Component Health::id = 2;
const Component Render::id = 3;

#endif