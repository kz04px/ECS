#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP


#include <iostream>
#include <vector>
#include <memory>


typedef uint32_t Entity;
typedef uint32_t Component;
class SystemManager;
class Manager;


class System
{
    public:
        virtual void update(const float dt) = 0;
        void removeEntity(const Entity e)
        {
            entities.erase(e);
        }
        std::set<Entity> entities;
        std::set<Component> required;
        Manager *manager;
};


class SystemManager
{
    public:
        SystemManager() : systems({})
        {
        }
        void update(const float dt)
        {
            for(auto &s : systems)
            {
                s->update(dt);
            }
        }
        void print()
        {
            std::cout << "SystemManager:" << std::endl;
            for(auto &s : systems)
            {
                std::cout << "  ";

                std::cout << "Entities:";
                for(auto e : s->entities)
                {
                    std::cout << " " << e;
                }

                std::cout << " Required:";
                for(auto c : s->required)
                {
                    std::cout << " " << c;
                }

                std::cout << std::endl;
            }
        }
        void removeEntity(const Entity e)
        {
            for(auto &system : systems)
            {
                system->removeEntity(e);
            }
        }
        void updateEntity(const Entity e, const std::set<Component> c)
        {
            for(auto &s : systems)
            {
                bool valid = true;
                for(auto r : s->required)
                {
                    if(c.find(r) == c.end())
                    {
                        valid = false;
                        break;
                    }
                }
                if(valid == true)
                {
                    s->entities.insert(e);
                }
            }
        }
        template<typename T>
        T* getSystem()
        {
            return NULL;
            /*
            for(auto s : systems)
            {
                if(s->id == T::id)
                {
                    return dynamic_cast<T*>(s);
                }
            }
            */
        }
        template<typename T>
        void addSystem(T* t)
        {
            static_assert(std::is_base_of<System, T>::value, "System must derive from System base class");
            systems.push_back(t);
        }
    private:
        std::vector<System*> systems;
};


#endif