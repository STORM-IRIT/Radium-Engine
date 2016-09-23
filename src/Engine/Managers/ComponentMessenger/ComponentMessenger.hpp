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
        /// output (get), input (set) or read/write functions on arbitrary type parameters.
        /// Then any component (typically belonging to the same entity) can try to access
        /// the given data through the functions.
        /// Declaring I/O is done by registering a function pointer with the correct
        /// prototype ( returning `const T*` for getters, accepting `const T*` for setters
        /// and returning a `T*` for read/write getters).
        /// Each function is identified by its entity, a name and the return type.
        /// Then, another component can try to query the messenger to check if
        /// a data of the relevant type, entity and string-id is available, with the
        /// canGet(), canSet() and canRw() functions.
        /// It is then possible to directly read or write this data with the get(), set()
        /// and rw() functions.
        /// For more efficiency the underlying function pointers are directly accessible
        /// as well and can be queried with the same identifiers.
        class RA_ENGINE_API ComponentMessenger
        {
        RA_SINGLETON_INTERFACE(ComponentMessenger);

        public:

            /// This describes the function pointers accepted for each type.
            template<typename T>
            struct CallbackTypes
            {
                /// Function pointer for a getter function.
                typedef std::function<const T*(void)> Getter;
                /// Function pointer for a setter function.
                typedef std::function<void(const T*)> Setter;

                /// Function pointer for a read/write getter.
                typedef std::function<T*(void)> ReadWrite;


                /// Calls the callback and retrieves the object
                static const T& getHelper( const Getter& g ) { return *(g());}
            };

            template <typename T>
            struct CallbackTypes<std::shared_ptr<T>>
            {
                typedef std::function<std::shared_ptr<const T>(void)> Getter;
                typedef std::function<void(std::shared_ptr<const T>)> Setter;
                typedef std::function<std::shared_ptr<T>(void)> ReadWrite;

                static const std::shared_ptr<const T>& getHelper( const Getter& g) { return (g()); }
            };

        private:
            /// Key used to identify entries.
            typedef std::pair<std::string, std::type_index> Key;

            // Unfortunately there is no standard hash functions for std::pair.
            // so we have to provide one (which justs xors the two hashes)
            // We could use a proper hash combination function like this one :
            // http://www.boost.org/doc/libs/1_46_1/doc/html/hash/reference.html#boost.hash_combine

            /// Hash function for our key type
            struct HashFunc
            {
                inline std::size_t operator()(const Key& k) const;
            };

            /// Class hierarchy for polymorphic storage of callback functions.
            struct CallbackBase
            {
            };
            template<typename T>
            struct GetterCallback : public CallbackBase
            {
                typename CallbackTypes<T>::Getter m_cb;
            };
            template<typename T>
            struct SetterCallback : public CallbackBase
            {
                typename CallbackTypes<T>::Setter m_cb;
            };
            template<typename T>
            struct RwCallback : public CallbackBase
            {
                typename CallbackTypes<T>::ReadWrite m_cb;
            };

            /// A dictionary of callback entries identified with the key.
            typedef std::unordered_map<Key, std::unique_ptr<CallbackBase>, HashFunc> CallbackMap;

        public:
            ComponentMessenger() { }

            //
            // Direct access to function pointers
            //

            template<typename ReturnType>
            inline typename CallbackTypes<ReturnType>::Getter getterCallback(const Entity* entity,
                                                                             const std::string& id);

            template<typename ReturnType>
            inline typename CallbackTypes<ReturnType>::ReadWrite rwCallback(const Entity* entity,
                                                                            const std::string& id);

            template<typename ReturnType>
            inline typename CallbackTypes<ReturnType>::Setter setterCallback(const Entity* entity,
                                                                             const std::string& id);

            //
            // Access the exported data.
            //

            // Note : all functions assert when the data is not available.
            // e.g. get() assert if canGet() is false.

            template<typename ReturnType>
            inline const ReturnType& get(const Entity* entity, const std::string& id);



            //
            // Query if a given key has registered data.
            //

            template<typename ReturnType>
            inline bool canGet(const Entity* entity, const std::string& id);

            template<typename ReturnType>
            inline bool canSet(const Entity* entity, const std::string& id);

            template<typename ReturnType>
            inline bool canRw(const Entity* entity, const std::string& id);


            //
            // Register callbacks
            //

            template<typename ReturnType>
            inline void registerOutput(const Entity* entity, Component* comp, const std::string& id,
                                       const typename CallbackTypes<ReturnType>::Getter& cb);

            template<typename ReturnType>
            inline void registerReadWrite(const Entity* entity, Component* comp, const std::string& id,
                                          const typename CallbackTypes<ReturnType>::ReadWrite& cb);

            template<typename ReturnType>
            inline void registerInput(const Entity* entity, Component* comp, const std::string& id,
                                      const typename CallbackTypes<ReturnType>::Setter& cb);

        private:
            std::unordered_map<const Entity*, CallbackMap> m_entityGetLists; /// Per-entity callback get list.
            std::unordered_map<const Entity*, CallbackMap> m_entitySetLists; /// Per-entity callback set list.
            std::unordered_map<const Entity*, CallbackMap> m_entityRwLists;  /// Per-entity callback read-write list.

        };

    }
}

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.inl>

#endif // RADIUMENGINE_COMPONENTMESSENGER_H__
