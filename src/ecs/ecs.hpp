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
        void addEntityComponent(Entity e, Component c)
        {
            em.entities[e].insert(c);
            cm.components[c].insert(e);
            cm.stores[c]->addEntity(e);
            sm.updateEntity(e, em.entities[e]);
        }
        template<typename T>
        void createComponent()
        {
            cm.addComponent<T>();
        }
        template<typename T>
        void createSystem()
        {
            sm.addSystem<T>(&em, &cm);
        }
        EntityManager em;
        ComponentManager cm;
        SystemManager sm;
    private:
};

#endif