#ifndef RADIUMENGINE_COMPONENTMESSENGER_H__
#define RADIUMENGINE_COMPONENTMESSENGER_H__

#include <Engine/RaEngine.hpp>

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>

#include <Core/Utils/Singleton.hpp>
#include <Engine/Entity/Component.hpp>



namespace Ra
{
namespace Engine
{
class Entity;
class Callback;
class Component;


/// This class allows components to communicate data to each other in a type safe way
/// When a component is created, after being attached to an entity, it can declare
/// output (get) or input (set) functions on arbitrary type parameters.
/// Then any component (typically belonging to the same entity) can try
/// use the get() or set() functions on the element.
/// A get or set entry is identified by its return type and a string which allows
/// the components to disambiguate if multiple components return the same type.
class RA_ENGINE_API ComponentMessenger
{
    RA_SINGLETON_INTERFACE( ComponentMessenger );

public:

    /// An entry which allows to call the get/set methods on a component.
    struct CallbackEntry
    {
        Component* m_component;
        void* cb;
    };

    /// Key used to identify entries. 
    typedef std::pair<std::string, std::type_index> Key;

    // Unfortunately there is no standard hash functions for std::pair.
    // so we have to provide one (which justs xors the two hashes)
    // We could use a proper hash combination function like this one : 
    // http://www.boost.org/doc/libs/1_46_1/doc/html/hash/reference.html#boost.hash_combine

    /// Hash function for our key type
    struct HashFunc
    {
        std::size_t operator() ( const Key& k ) const
        {
            return (std::hash<std::string>()(k.first) ^ std::hash<std::type_index>()(k.second));
        }
    };

    /// A dictionary of callback entries identified with the key.
    typedef std::unordered_map < Key, CallbackEntry, HashFunc > EntityCallbackList;

public:
    ComponentMessenger() {}

    /// Attempts to get the data identified by the given string. If the data is found,
    /// the function returns true and output is correctly set. If not, the function
    /// returns false.
    template < typename ReturnType >
    bool get( const Entity* entity, const std::string& id, ReturnType& output )
    {
        // Attempt to find the given entity list.
        const auto& entityListPos = m_entityLists.find( entity );
        CORE_ASSERT( entityListPos != m_entityLists.end(), " Entity has no registered component" );

        Key key ( id,  std::type_index( typeid(ReturnType) ));
        const EntityCallbackList& entityList = entityListPos->second;


        // Check if there are components exporting the given type,
        // so let's try to find if there is one with the requested id.
        const auto& callbackEntry = entityList.find( key );
        const bool found = (callbackEntry != entityList.end());
        if ( found )
        {
            printf( "gotcha %s\n", callbackEntry->second.m_component->getName().c_str() );
        }

        return found;
    }

    /// Register an output parameter, which becomes accessible by get() functions with the
    /// given string as an identifier.
    template <typename ReturnType>
    void registerOutput(const Entity* entity, Component* comp, const std::string& id, void* cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        EntityCallbackList& entityList = m_entityLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "exists" );

        CallbackEntry entry =  { comp, cb };
        entityList[key] = entry;
    }

    std::unordered_map<const Entity*, EntityCallbackList> m_entityLists; /// Per-entity callback list.

};

}
}



#endif // RADIUMENGINE_COMPONENTMESSENGER_H__
