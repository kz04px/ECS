#ifndef ECS_HPP
#define ECS_HPP


#include "entity_manager.hpp"
#include "component_manager.hpp"
#include "system_manager.hpp"


class Manager
{
    public:
        Manager() : em(EntityManager()), cm(ComponentManager()), sm(SystemManager())
        {
        }
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
            assert(e != invalidEntity);

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
            t->sm = &sm;
            sm.addSystem<T>(t);
        }
        EntityManager em;
        ComponentManager cm;
        SystemManager sm;
    private:
};


#endif