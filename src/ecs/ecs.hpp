#ifndef ECS_HPP
#define ECS_HPP

#include "entity_manager.hpp"
#include "component_manager.hpp"
#include "system_manager.hpp"

class Manager
{
    public:
        void print()
        {
            em.print();
            std::cout << std::endl;
            cm.print();
            std::cout << std::endl;
            sm.print();
            std::cout << std::endl;
        }
        template<typename T>
        void addEntityComponent(Entity e, T t)
        {
            em.allEntities.insert(e);
            em.entities[e].insert(T::id);
            cm.components[T::id].insert(e);
            cm.getStore<T>().addEntity(e, t);
            sm.updateEntity(e, em.entities[e]);
        }
        template<typename T>
        void createComponent()
        {
            cm.addComponent<T>();
        }
        template<typename T>
        void createSystem(T* t)
        {
            t->em = &em;
            t->cm = &cm;
            sm.addSystem<T>(t);
        }
        EntityManager em;
        ComponentManager cm;
        SystemManager sm;
    private:
};

#endif