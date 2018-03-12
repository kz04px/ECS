#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP


#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>


#define MAX_ENTITIES 1000000


typedef uint32_t Entity;
typedef uint32_t Component;
const Entity invalidEntity = 0;


class EntityManager
{
    public:
        EntityManager() : allEntities({}), entities({})
        {
        }
        void add(std::set<Component> components)
        {
            entities[next] = components;
            next++;
        }
        void removeEntity(const Entity e)
        {
            assert(e != invalidEntity);

            allEntities.erase(e);
            entities.erase(e);
        }
        void print()
        {
            std::cout << "EntityManager:" << std::endl;
            for(auto e : entities)
            {
                std::cout << "  " << e.first << ":";
                for(auto c : e.second)
                {
                    std::cout << " " << c;
                }
                std::cout << std::endl;
            }
        }
        Entity getEntity()
        {
            if(next >= MAX_ENTITIES)
            {
                return invalidEntity;
            }
            return next++;
        }
        Entity next = 1;
        std::set<Entity> allEntities;
        std::unordered_map<Entity, std::set<Component>> entities;
    private:
};


#endif