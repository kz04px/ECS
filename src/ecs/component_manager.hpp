#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include <iostream>
#include <set>
#include <unordered_map>
#include <memory>

typedef uint32_t Entity;
typedef uint32_t Component;

class Store
{
    public:
        virtual ~Store() = default;
        virtual void remove_entity(const Entity e) = 0;
};

template<typename T>
class ComponentStore : public Store
{
    public:
        ComponentStore(const Component id_) : Store(), id(id_)
        {
        }
        void add_entity(const Entity e, T t)
        {
            components.insert(std::pair<Entity, T>(e, t));
        }
        void remove_entity(const Entity e)
        {
            components.erase(e);
        }
        T* get_component(const Entity e)
        {
            return &components[e];
        }
        void print()
        {
            std::cout << "ComponentStore:" << std::endl;
            for(auto i : components)
            {
                std::cout << i.first << ": " << i.second.x << "," << i.second.y << std::endl;
            }
        }
        const Component id;
        std::unordered_map<Entity, T> components;
    private:
};

class ComponentManager
{
    public:
        ComponentManager()
        {
        }
        void print()
        {
            std::cout << "ComponentManager:" << std::endl;

            std::cout << "  Components:" << std::endl;
            for(auto c : components)
            {
                std::cout << "    " << c.first << ":";
                for(auto e : c.second)
                {
                    std::cout << " " << e;
                }
                std::cout << std::endl;
            }

            std::cout << "  Stores:" << std::endl;
            for(auto &s : stores)
            {
                std::cout << "    " << s.first << ":";
                std::cout << std::endl;
            }
        }
        template<typename T>
        void add_component()
        {
            stores[T::id].reset(static_cast<Store*>(new ComponentStore<T>(T::id)));
        }
        void remove_entity(const Entity e)
        {
            for(auto &component : components)
            {
                component.second.erase(e);
            }
            for(auto &store : stores)
            {
                store.second->remove_entity(e);
            }
        }
        template<typename T>
        ComponentStore<T>& get_store()
        {
            return dynamic_cast<ComponentStore<T>&>(*stores[T::id]);
        }
        bool entity_has_component(const Entity e, const Component c)
        {
            return components[c].find(e) != components[c].end();
        }
        Component next = 0;
        std::unordered_map<Component, std::set<Entity>> components;
        std::unordered_map<Component, std::unique_ptr<Store>> stores;
    private:
};

#endif
