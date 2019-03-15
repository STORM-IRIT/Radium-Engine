#ifndef RADIUMENGINE_COMPONENTMESSENGER_H__
#define RADIUMENGINE_COMPONENTMESSENGER_H__

#include <Engine/RaEngine.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <Core/Utils/Singleton.hpp>
#include <Engine/Component/Component.hpp>

namespace Ra {
namespace Engine {
class Entity;
class Component;

/**
 * This class allows components to communicate data to each other in a type safe way.
 * When a component is created, after being attached to an entity, it can declare
 * output (get), input (set) or read/write functions on arbitrary type parameters.
 * Then any component (typically belonging to the same entity) can try to access
 * the given data through the functions.
 *
 * Declaring I/O is done by registering a function pointer with the correct
 * prototype ( returning `const T*` for getters, accepting `const T*` for setters
 * and returning a `T*` for read/write getters).
 * Each function is identified by its entity, a name and the return type.
 * Then, another component can try to query the messenger to check if
 * a data of the relevant type, entity and string-id is available, with the
 * canGet(), canSet() and canRw() functions.
 *
 * It is then possible to directly read or write this data with the get(), set()
 * and rw() functions.
 *
 * For more efficiency the underlying function pointers are directly accessible
 * as well and can be queried with the same identifiers.
 */
class RA_ENGINE_API ComponentMessenger {
    RA_SINGLETON_INTERFACE( ComponentMessenger );

  public:
    /**
     * This describes the function pointers accepted for each type.
     */
    template <typename T>
    struct CallbackTypes {
        /**
         * Function pointer for a getter function.
         */
        using Getter = std::function<const T*( void )>;

        /**
         * Function pointer for a setter function.
         */
        using Setter = std::function<void( const T* )>;

        /**
         * Function pointer for a read/write getter.
         */
        using ReadWrite = std::function<T*( void )>;

        /**
         * Calls the callback and retrieves the object.
         */
        static const T& getHelper( const Getter& g ) { return *( g() ); }
    };

    /**
     * std::shared_ptr version.
     */
    template <typename T>
    struct CallbackTypes<std::shared_ptr<T>> {
        using Getter = std::function<std::shared_ptr<const T>( void )>;
        using Setter = std::function<void( std::shared_ptr<const T> )>;
        using ReadWrite = std::function<std::shared_ptr<T>( void )>;

        static const std::shared_ptr<const T>& getHelper( const Getter& g ) { return ( g() ); }
    };

  private:
    /**
     * Key used to identify entries.
     */
    using Key = std::pair<std::string, std::type_index>;

    // Unfortunately there is no standard hash functions for std::pair.
    // so we have to provide one (which justs xors the two hashes)
    // We could use a proper hash combination function like this one :
    // http://www.boost.org/doc/libs/1_46_1/doc/html/hash/reference.html#boost.hash_combine

    /**
     * Hash function for our key type.
     */
    struct HashFunc {
        inline std::size_t operator()( const Key& k ) const;
    };

    /// \name Class hierarchy for polymorphic storage of callback functions.
    /// \{

    /**
     * Base class for callbacks.
     */
    struct CallbackBase {};

    /**
     * Getter callbacks.
     */
    template <typename T>
    struct GetterCallback : public CallbackBase {
        typename CallbackTypes<T>::Getter m_cb;
    };

    /**
     * Setter callbacks.
     */
    template <typename T>
    struct SetterCallback : public CallbackBase {
        typename CallbackTypes<T>::Setter m_cb;
    };

    /**
     * Read-Write callbacks.
     */
    template <typename T>
    struct RwCallback : public CallbackBase {
        typename CallbackTypes<T>::ReadWrite m_cb;
    };
    /// \}

    /**
     * A dictionary of callback entries identified with a key based on the data type.
     */
    using CallbackMap = std::unordered_map<Key, std::unique_ptr<CallbackBase>, HashFunc>;

  public:
    ComponentMessenger() = default;

    /** \name Access callbacks
     * \note All these functions assert if the getter is not available.
     */
    /// \{

    /**
     * Return the getter callback associated to the given Entity and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline typename CallbackTypes<ReturnType>::Getter getterCallback( const Entity* entity,
                                                                      const std::string& id );

    /**
     * Return the read-write callback associated to the given Entity and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline typename CallbackTypes<ReturnType>::ReadWrite rwCallback( const Entity* entity,
                                                                     const std::string& id );

    /**
     * Return the setter callback associated to the given Entity and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline typename CallbackTypes<ReturnType>::Setter setterCallback( const Entity* entity,
                                                                      const std::string& id );

    /**
     * Return the data accessed via the getter callback associated to the given
     * Entity and data id.
     */
    // FIXME: should we keep this one?
    template <typename ReturnType>
    inline const ReturnType& get( const Entity* entity, const std::string& id );
    /// \}

    /// \name Status query
    /// \{

    /**
     * Return true if a getter callback has been registered for the given Entity
     * and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline bool canGet( const Entity* entity, const std::string& id );

    /**
     * Return true if a setter callback has been registered for the given Entity
     * and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline bool canSet( const Entity* entity, const std::string& id );

    /**
     * Return true if a read-write callback has been registered for the given
     * Entity and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline bool canRw( const Entity* entity, const std::string& id );
    /// \}

    /// \name Callbacks registration
    // FIXME: Shall we deal with unregistration?
    /// \{

    /**
     * Register \p cb as the getter callback associated to the given Entity and
     * data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline void registerOutput( const Entity* entity, Component* comp, const std::string& id,
                                const typename CallbackTypes<ReturnType>::Getter& cb );

    /**
     * Register \p cb as the read-write callback associated to the given Entity
     * and data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline void registerReadWrite( const Entity* entity, Component* comp, const std::string& id,
                                   const typename CallbackTypes<ReturnType>::ReadWrite& cb );

    /**
     * Register \p cb as the setter callback associated to the given Entity and
     * data id.
     * \tparam ReturnType type of the data.
     */
    template <typename ReturnType>
    inline void registerInput( const Entity* entity, Component* comp, const std::string& id,
                               const typename CallbackTypes<ReturnType>::Setter& cb );
    /// \}

  private:
    /// Per-entity getter callback list.
    std::unordered_map<const Entity*, CallbackMap> m_entityGetLists;

    /// Per-entity setter callback list.
    std::unordered_map<const Entity*, CallbackMap> m_entitySetLists;

    /// Per-entity read-write callback list.
    std::unordered_map<const Entity*, CallbackMap> m_entityRwLists;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.inl>

#endif // RADIUMENGINE_COMPONENTMESSENGER_H__
