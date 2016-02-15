#ifndef RADIUMENGINE_COMPONENTMESSENGER_H__
#define RADIUMENGINE_COMPONENTMESSENGER_H__

#include <Engine/RaEngine.hpp>

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <iostream>

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
    typedef std::function< const void*(void) > GetterCallback;
    typedef std::function< void(const void*) > SetterCallback;
    typedef std::function< void* (void)      > ReadWriteCallback;

    /// An entry which allows to call the get/set methods on a component.
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
    typedef std::unordered_map < Key, GetterCallback,    HashFunc > EntityGetterCallbackList;
    typedef std::unordered_map < Key, SetterCallback,    HashFunc > EntitySetterCallbackList;
    typedef std::unordered_map < Key, ReadWriteCallback, HashFunc > EntityRwCallbackList;

public:
    ComponentMessenger() {}

    template <typename ReturnType>
    GetterCallback getterCallback( const Entity* entity, const std::string id )
    {
        return m_entityGetLists[entity][Key(id, std::type_index(typeid(ReturnType)))];
    }

    template <typename ReturnType>
    ReadWriteCallback rwCallback( const Entity* entity, const std::string id )
    {
        return m_entityRwLists[entity][Key(id, std::type_index(typeid(ReturnType)))];
    }

    template <typename ReturnType>
    SetterCallback setterCallback( const Entity* entity, const std::string id )
    {
        return m_entitySetLists[entity][Key(id, std::type_index(typeid(ReturnType)))];
    }

    /// Attempts to get the data identified by the given string. If the data is found,
    /// the function returns true and output is correctly set. If not, the function
    /// returns false.
    template < typename ReturnType >
    bool get( const Entity* entity, const std::string& id, ReturnType& output )
    {
        // Attempt to find the given entity list.
        const auto& entityListPos = m_entityGetLists.find( entity );
        CORE_ASSERT( entityListPos != m_entityGetLists.end(), " Entity has no registered component" );

        Key key ( id,  std::type_index( typeid(ReturnType) ));
        const EntityGetterCallbackList& entityList = entityListPos->second;


        // Check if there are components exporting the given type,
        // so let's try to find if there is one with the requested id.
        const auto& callbackEntry = entityList.find( key );
        const bool found = (callbackEntry != entityList.end());
        if ( found )
        {
            const ReturnType* value = static_cast<const ReturnType*>(callbackEntry->second());
            output = *value;
        }

        return found;
    }

    /// Attempts to set the data identified by the given string. If the data is found,
    /// the function returns true and the comonent has accepted the input parameter.
    ///  If not, the function returns false.
    template < typename ReturnType >
    bool set( const Entity* entity, const std::string& id, const ReturnType& input )
    {
        // Attempt to find the given entity list.
        const auto& entityListPos = m_entitySetLists.find( entity );
        CORE_ASSERT( entityListPos != m_entitySetLists.end(), " Entity has no registered component" );

        Key key ( id,  std::type_index( typeid(ReturnType) ));
        const EntitySetterCallbackList& entityList = entityListPos->second;


        // Check if there are components exporting the given type,
        // so let's try to find if there is one with the requested id.
        const auto& callbackEntry = entityList.find( key );
        const bool found = (callbackEntry != entityList.end());
        if ( found )
        {
            callbackEntry->second(&input);
        }

        return found;
    }

    /// Register an output parameter, which becomes accessible by getter callback functions
    /// given string as an identifier.
    template <typename ReturnType>
    void registerOutput(const Entity* entity, Component* comp, const std::string& id, const GetterCallback& cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        EntityGetterCallbackList& entityList = m_entityGetLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "Output function alreadu registered" );

        entityList[key] = cb;
    }

    template <typename ReturnType>
    void registerReadWrite(const Entity* entity, Component* comp, const std::string& id, const ReadWriteCallback& cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        EntityRwCallbackList& entityList = m_entityRwLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "Rw function already registered" );

        entityList[key] = cb;
    }

    template <typename ReturnType>
    void registerInput(const Entity* entity, Component* comp, const std::string& id, const SetterCallback& cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        EntitySetterCallbackList& entityList = m_entitySetLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "Input function already registered" );

        entityList[key] = cb;
    }

    std::unordered_map<const Entity*, EntityGetterCallbackList> m_entityGetLists; /// Per-entity callback get list.
    std::unordered_map<const Entity*, EntitySetterCallbackList> m_entitySetLists; /// Per-entity callback set list.
    std::unordered_map<const Entity*, EntityRwCallbackList> m_entityRwLists;  /// Per-entity callback read-write list.

};

}
}



#endif // RADIUMENGINE_COMPONENTMESSENGER_H__
