#ifndef RADIUMENGINE_COMPONENTMESSENGER_H__
#define RADIUMENGINE_COMPONENTMESSENGER_H__

#include <Engine/RaEngine.hpp>

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <iostream>

#include <Core/Utils/Singleton.hpp>
#include <Engine/Component/Component.hpp>

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

    struct CallbackBase {};

    template <typename T>
    struct CallbackTypes
    {
        typedef std::function<const T *(void)> Getter;
        typedef std::function<void(const T *)> Setter;
        typedef std::function<T *(void)> ReadWrite;
    };

    template <typename T>
    struct GetterCallback : public CallbackBase
    {
        typename CallbackTypes<T>::Getter m_cb;
    };

    template <typename T>
    struct SetterCallback : public CallbackBase
    {
        typename CallbackTypes<T>::Setter m_cb;
    };

    template <typename T>
    struct RwCallback : public CallbackBase
    {
        typename CallbackTypes<T>::ReadWrite m_cb;
    };

    /// A dictionary of callback entries identified with the key.
    typedef std::unordered_map < Key, std::unique_ptr<CallbackBase>, HashFunc > CallbackMap;

public:
    ComponentMessenger() {}

    template <typename ReturnType>
    typename CallbackTypes<ReturnType>::Getter getterCallback( const Entity* entity, const std::string& id )
    {
        CORE_ASSERT( canGet<ReturnType>( entity, id), "Unregistered callback");
        return static_cast<GetterCallback<ReturnType>*>(m_entityGetLists[entity][Key(id, std::type_index(typeid(ReturnType)))].get())->m_cb;
    }

    template <typename ReturnType>
    typename CallbackTypes<ReturnType>::ReadWrite rwCallback( const Entity* entity, const std::string& id )
    {
        CORE_ASSERT( canRw<ReturnType>( entity, id), "Unregistered callback");
        return static_cast<RwCallback<ReturnType>*>(m_entityRwLists[entity][Key(id, std::type_index(typeid(ReturnType)))].get())->m_cb;
    }

    template <typename ReturnType>
    typename CallbackTypes<ReturnType>::Setter setterCallback( const Entity* entity, const std::string& id )
    {
        CORE_ASSERT( canSet<ReturnType>( entity, id), "Unregistered callback");
        return static_cast<SetterCallback<ReturnType>*>(m_entitySetLists[entity][Key(id, std::type_index(typeid(ReturnType)))].get())->m_cb;
    }

    template<typename ReturnType> const ReturnType& get( const Entity* entity, const std::string& id )
    {
        return *(getterCallback<ReturnType>(entity, id)());
    }

    template<typename ReturnType> void set( const Entity* entity, const std::string& id, const ReturnType& x )
    {
        return setterCallback<ReturnType>(entity, id)(x);
    }

    template <typename ReturnType> ReturnType& rw( const Entity* entity, const std::string& id)
    {
        return *(getterCallback<ReturnType>(entity, id)());
    }

    /// Returns true if data with given entity, id and type are accessible through
    /// a getter callback.
    template < typename ReturnType >
    bool canGet( const Entity* entity, const std::string& id )
    {
        // Attempt to find the given entity list.
        const auto& entityListPos = m_entityGetLists.find( entity );
        CORE_ASSERT( entityListPos != m_entityGetLists.end(), " Entity has no registered component" );

        Key key ( id,  std::type_index( typeid(ReturnType) ));
        const CallbackMap& entityList = entityListPos->second;

        // Check if there are components exporting the given type,
        // so let's try to find if there is one with the requested id.
        const auto& callbackEntry = entityList.find( key );
        const bool found = (callbackEntry != entityList.end());
        return found;
    }

    /// Returns true if data with given entity, id and type are writable through
    /// a writer callback.
    template < typename ReturnType >
    bool canSet( const Entity* entity, const std::string& id )
    {
        // Attempt to find the given entity list.
        const auto& entityListPos = m_entitySetLists.find( entity );
        CORE_ASSERT( entityListPos != m_entitySetLists.end(), " Entity has no registered component" );

        Key key ( id,  std::type_index( typeid(ReturnType) ));
        const CallbackMap& entityList = entityListPos->second;

        // Check if there are components exporting the given type,
        // so let's try to find if there is one with the requested id.
        const auto& callbackEntry = entityList.find( key );
        const bool found = (callbackEntry != entityList.end());
        return found;
    }

    /// Returns true if data with given entity, id and type are accessible through
    /// a read/write callback.
    template < typename ReturnType >
    bool canRw( const Entity* entity, const std::string& id )
    {
        // Attempt to find the given entity list.
        const auto& entityListPos = m_entityRwLists.find( entity );
        CORE_ASSERT( entityListPos != m_entityRwLists.end(), " Entity has no registered component" );

        Key key ( id,  std::type_index( typeid(ReturnType) ));
        const CallbackMap& entityList = entityListPos->second;

        // Check if there are components exporting the given type,
        // so let's try to find if there is one with the requested id.
        const auto& callbackEntry = entityList.find( key );
        const bool found = (callbackEntry != entityList.end());
        return found;
    }

    /// Register an output parameter, which becomes accessible by getter callback functions
    /// given string as an identifier.
    template <typename ReturnType>
    void registerOutput(const Entity* entity, Component* comp, const std::string& id, const typename CallbackTypes<ReturnType>::Getter& cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        CallbackMap& entityList = m_entityGetLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "Output function already registered" );

        GetterCallback<ReturnType>* getter = new GetterCallback<ReturnType>();
        getter->m_cb = cb;
        entityList[key].reset(getter);
    }

    template <typename ReturnType>
    void registerReadWrite(const Entity* entity, Component* comp, const std::string& id, const typename CallbackTypes<ReturnType>::ReadWrite& cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        CallbackMap& entityList = m_entityRwLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "Rw function already registered" );

        RwCallback<ReturnType>* rw = new RwCallback<ReturnType>();
        rw->m_cb = cb;
        entityList[key].reset(rw);
    }

    template <typename ReturnType>
    void registerInput(const Entity* entity, Component* comp, const std::string& id, const typename CallbackTypes<ReturnType>::Setter& cb )
    {
        CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
        // Will insert a new entity entry if it doesn't exist.
        CallbackMap& entityList = m_entitySetLists[entity];

        Key key( id, std::type_index( typeid(ReturnType) ) );
        CORE_ASSERT( entityList.find(key) == entityList.end(), "Input function already registered" );

        SetterCallback<ReturnType>* setter = new SetterCallback<ReturnType>();
        setter->m_cb = cb;
        entityList[key].reset(setter);
    }

    std::unordered_map<const Entity*, CallbackMap> m_entityGetLists; /// Per-entity callback get list.
    std::unordered_map<const Entity*, CallbackMap> m_entitySetLists; /// Per-entity callback set list.
    std::unordered_map<const Entity*, CallbackMap> m_entityRwLists;  /// Per-entity callback read-write list.

};

}
}



#endif // RADIUMENGINE_COMPONENTMESSENGER_H__
