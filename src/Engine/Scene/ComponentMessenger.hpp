#pragma once

#include <Engine/RaEngine.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <Core/Utils/Singleton.hpp>
#include <Engine/Scene/Component.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

class Entity;
class Component;

/** This class allows components to communicate data to each other in a type safe way
 * When a component is created, after being attached to an entity, it can declare
 * output (get), input (set) or read/write functions on arbitrary type parameters.
 * Then any component (typically belonging to the same entity) can try to access
 * the given data through the functions.
 * Declaring I/O is done by registering a function pointer with the correct
 * prototype ( returning `const T*` for getters, accepting `const T*` for setters
 * and returning a `T*` for read/write getters).
 * Each function is identified by its entity, a name and the return type.
 * Then, another component can try to query the messenger to check if
 * a data of the relevant type, entity and string-id is available, with the
 * canGet(), canSet() and canRw() functions.
 * It is then possible to directly read or write this data with the get(), set()
 * and rw() functions.
 * For more efficiency the underlying function pointers are directly accessible
 * as well and can be queried with the same identifiers.
 */
class RA_ENGINE_API ComponentMessenger
{
    RA_SINGLETON_INTERFACE( ComponentMessenger );

  public:
    /// This describes the function pointers accepted for each type.
    template <typename T>
    struct CallbackTypes {
        /// Function pointer for a getter function.
        using Getter = std::function<const T*( void )>;
        /// Function pointer for a setter function.
        using Setter = std::function<void( const T* )>;

        /// Function pointer for a read/write getter.
        using ReadWrite = std::function<T*( void )>;

        /// Calls the callback and retrieves the object
        static const T& getHelper( const Getter& g ) { return *( g() ); }
    };

    template <typename T>
    struct CallbackTypes<std::shared_ptr<T>> {
        using Getter    = std::function<std::shared_ptr<const T>( void )>;
        using Setter    = std::function<void( std::shared_ptr<const T> )>;
        using ReadWrite = std::function<std::shared_ptr<T>( void )>;

        static const std::shared_ptr<const T>& getHelper( const Getter& g ) { return ( g() ); }
    };

  private:
    /// Key used to identify entries.
    using Key = std::pair<std::string, std::type_index>;

    // Unfortunately there is no standard hash functions for std::pair.
    // so we have to provide one (which justs xors the two hashes)
    // We could use a proper hash combination function like this one :
    // http://www.boost.org/doc/libs/1_46_1/doc/html/hash/reference.html#boost.hash_combine

    /// Hash function for our key type
    struct HashFunc {
        inline std::size_t operator()( const Key& k ) const;
    };

    /// Class hierarchy for polymorphic storage of callback functions.
    struct CallbackBase {};
    template <typename T>
    struct GetterCallback : public CallbackBase {
        typename CallbackTypes<T>::Getter m_cb;
    };
    template <typename T>
    struct SetterCallback : public CallbackBase {
        typename CallbackTypes<T>::Setter m_cb;
    };
    template <typename T>
    struct RwCallback : public CallbackBase {
        typename CallbackTypes<T>::ReadWrite m_cb;
    };

    /// A dictionary of callback entries identified with the key.
    using CallbackMap = std::unordered_map<Key, std::unique_ptr<CallbackBase>, HashFunc>;

  public:
    ComponentMessenger() = default;

    //
    // Direct access to function pointers
    //

    template <typename ReturnType>
    inline typename CallbackTypes<ReturnType>::Getter getterCallback( const Entity* entity,
                                                                      const std::string& id );

    template <typename ReturnType>
    inline typename CallbackTypes<ReturnType>::ReadWrite rwCallback( const Entity* entity,
                                                                     const std::string& id );

    template <typename ReturnType>
    inline typename CallbackTypes<ReturnType>::Setter setterCallback( const Entity* entity,
                                                                      const std::string& id );

    //
    // Access the exported data.
    //

    // Note : all functions assert when the data is not available.
    // e.g. get() assert if canGet() is false.

    template <typename ReturnType>
    inline const ReturnType& get( const Entity* entity, const std::string& id );

    //
    // Query if a given key has registered data.
    //

    template <typename ReturnType>
    inline bool canGet( const Entity* entity, const std::string& id );

    template <typename ReturnType>
    inline bool canSet( const Entity* entity, const std::string& id );

    template <typename ReturnType>
    inline bool canRw( const Entity* entity, const std::string& id );

    //
    // Register callbacks
    //

    template <typename ReturnType>
    inline void registerOutput( const Entity* entity,
                                Component* comp,
                                const std::string& id,
                                const typename CallbackTypes<ReturnType>::Getter& cb );

    template <typename ReturnType>
    inline void registerReadWrite( const Entity* entity,
                                   Component* comp,
                                   const std::string& id,
                                   const typename CallbackTypes<ReturnType>::ReadWrite& cb );

    template <typename ReturnType>
    inline void registerInput( const Entity* entity,
                               Component* comp,
                               const std::string& id,
                               const typename CallbackTypes<ReturnType>::Setter& cb );

    /// unregister all callbacks attached to \b entity's \b component
    void unregisterAll( const Entity* entity, Component* component );

  private:
    using EntityMap = std::unordered_map<const Entity*, CallbackMap>;
    EntityMap m_entityGetLists; /// Per-entity callback get list.
    EntityMap m_entitySetLists; /// Per-entity callback set list.
    EntityMap m_entityRwLists;  /// Per-entity callback read-write list.
};

} // namespace Scene
} // namespace Engine
} // namespace Ra

#include <Core/Utils/StdUtils.hpp>
namespace Ra {
namespace Engine {
namespace Scene {

inline std::size_t ComponentMessenger::HashFunc::operator()( const Key& k ) const {
    return Core::Utils::hash( k );
}

template <typename ReturnType>
inline typename ComponentMessenger::CallbackTypes<ReturnType>::Getter
ComponentMessenger::getterCallback( const Entity* entity, const std::string& id ) {
    CORE_ASSERT( canGet<ReturnType>( entity, id ), "Unregistered callback" );
    return static_cast<GetterCallback<ReturnType>*>(
               m_entityGetLists[entity][Key( id, std::type_index( typeid( ReturnType ) ) )].get() )
        ->m_cb;
}

template <typename ReturnType>
inline typename ComponentMessenger::CallbackTypes<ReturnType>::ReadWrite
ComponentMessenger::rwCallback( const Entity* entity, const std::string& id ) {
    CORE_ASSERT( canRw<ReturnType>( entity, id ), "Unregistered callback" );
    return static_cast<RwCallback<ReturnType>*>(
               m_entityRwLists[entity][Key( id, std::type_index( typeid( ReturnType ) ) )].get() )
        ->m_cb;
}

template <typename ReturnType>
inline typename ComponentMessenger::CallbackTypes<ReturnType>::Setter
ComponentMessenger::setterCallback( const Entity* entity, const std::string& id ) {
    CORE_ASSERT( canSet<ReturnType>( entity, id ), "Unregistered callback" );
    return static_cast<SetterCallback<ReturnType>*>(
               m_entitySetLists[entity][Key( id, std::type_index( typeid( ReturnType ) ) )].get() )
        ->m_cb;
}

template <typename ReturnType>
inline const ReturnType& ComponentMessenger::get( const Entity* entity, const std::string& id ) {
    return CallbackTypes<ReturnType>::getHelper( getterCallback<ReturnType>( entity, id ) );
}
/*
        template<typename ReturnType>
        inline void ComponentMessenger::set(const Entity* entity, const std::string& id, const
   ReturnType& x)
        {
            return setterCallback<ReturnType>(entity, id)(x);
        }
*/
template <typename ReturnType>
inline bool ComponentMessenger::canGet( const Entity* entity, const std::string& id ) {
    // Attempt to find the given entity list.
    const auto& entityListPos = m_entityGetLists.find( entity );
    if ( entityListPos == m_entityGetLists.end() )
        return false; // Entity has no registered component

    Key key( id, std::type_index( typeid( ReturnType ) ) );
    const CallbackMap& entityList = entityListPos->second;

    // Check if there are components exporting the given type,
    // so let's try to find if there is one with the requested id.
    const auto& callbackEntry = entityList.find( key );
    const bool found          = ( callbackEntry != entityList.end() );
    return found;
}

template <typename ReturnType>
inline bool ComponentMessenger::canSet( const Entity* entity, const std::string& id ) {
    // Attempt to find the given entity list.
    auto entityListPos = m_entitySetLists.find( entity );
    if ( entityListPos == m_entitySetLists.end() )
        return false; // Entity has no registered component

    Key key( id, std::type_index( typeid( ReturnType ) ) );
    const CallbackMap& entityList = entityListPos->second;

    // Check if there are components exporting the given type,
    // so let's try to find if there is one with the requested id.
    const auto& callbackEntry = entityList.find( key );
    const bool found          = ( callbackEntry != entityList.end() );
    return found;
}

template <typename ReturnType>
inline bool ComponentMessenger::canRw( const Entity* entity, const std::string& id ) {
    // Attempt to find the given entity list.
    const auto& entityListPos = m_entityRwLists.find( entity );
    if ( entityListPos == m_entityRwLists.end() )
        return false; // Entity has no registered component

    Key key( id, std::type_index( typeid( ReturnType ) ) );
    const CallbackMap& entityList = entityListPos->second;

    // Check if there are components exporting the given type,
    // so let's try to find if there is one with the requested id.
    const auto& callbackEntry = entityList.find( key );
    const bool found          = ( callbackEntry != entityList.end() );
    return found;
}

template <typename ReturnType>
inline void
ComponentMessenger::registerOutput( const Entity* entity,
                                    Component* comp,
                                    const std::string& id,
                                    const typename CallbackTypes<ReturnType>::Getter& cb ) {
    CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
    CORE_UNUSED( comp );
    // Will insert a new entity entry if it doesn't exist.
    CallbackMap& entityList = m_entityGetLists[entity];

    Key key( id, std::type_index( typeid( ReturnType ) ) );
    CORE_ASSERT( entityList.find( key ) == entityList.end(),
                 "Output function already registered for " + id );

    GetterCallback<ReturnType>* getter = new GetterCallback<ReturnType>();
    getter->m_cb                       = cb;
    entityList[key].reset( getter );
}

template <typename ReturnType>
inline void
ComponentMessenger::registerReadWrite( const Entity* entity,
                                       Component* comp,
                                       const std::string& id,
                                       const typename CallbackTypes<ReturnType>::ReadWrite& cb ) {
    CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
    CORE_UNUSED( comp );
    // Will insert a new entity entry if it doesn't exist.
    CallbackMap& entityList = m_entityRwLists[entity];

    Key key( id, std::type_index( typeid( ReturnType ) ) );
    CORE_ASSERT( entityList.find( key ) == entityList.end(),
                 "Rw function already registered for " + id );

    RwCallback<ReturnType>* rw = new RwCallback<ReturnType>();
    rw->m_cb                   = cb;
    entityList[key].reset( rw );
}

template <typename ReturnType>
inline void
ComponentMessenger::registerInput( const Entity* entity,
                                   Component* comp,
                                   const std::string& id,
                                   const typename CallbackTypes<ReturnType>::Setter& cb ) {
    CORE_ASSERT( entity && comp->getEntity() == entity, "Component not added to entity" );
    CORE_UNUSED( comp );
    // Will insert a new entity entry if it doesn't exist.
    CallbackMap& entityList = m_entitySetLists[entity];

    Key key( id, std::type_index( typeid( ReturnType ) ) );
    CORE_ASSERT( entityList.find( key ) == entityList.end(),
                 "Input function already registered for " + id );

    SetterCallback<ReturnType>* setter = new SetterCallback<ReturnType>();
    setter->m_cb                       = cb;
    entityList[key].reset( setter );
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
