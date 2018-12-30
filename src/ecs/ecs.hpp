#ifndef ECS_HPP
#define ECS_HPP

#include "entity_manager.hpp"
#include "component_manager.hpp"
#include "system_manager.hpp"

class Manager
{
    public:
        Manager() : em(EntityManager()), cm(ComponentManager()), sm(SystemManager()), remove({})
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
        void add_entity_component(Entity e, T t)
        {
            assert(e != invalid_entity);

            em.all_entities.insert(e);
            em.entities[e].insert(T::id);
            cm.components[T::id].insert(e);
            cm.get_store<T>().add_entity(e, t);
            sm.update_entity(e, em.entities[e]);
        }
        template<typename T>
        T* get_entity_component(const Entity e)
        {
            return cm.get_store<T>().get_component(e);
        }
        template<typename T>
        void create_component()
        {
            cm.add_component<T>();
        }
        template<typename T>
        void create_system(T* t)
        {
            t->manager = this;
            sm.add_system<T>(t);
        }
        void update(const float dt)
        {
            sm.update(dt);

            for(auto e : remove)
            {
                em.remove_entity(e);
                cm.remove_entity(e);
                sm.remove_entity(e);
            }
            remove.clear();
        }
        EntityManager em;
        ComponentManager cm;
        SystemManager sm;
        std::vector<Entity> remove;
    private:
};

#endif
