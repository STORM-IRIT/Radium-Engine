#include "ComponentMessenger.hpp"
#include <Core/Utils/StdUtils.hpp>
namespace Ra
{
    namespace Engine
    {

        inline std::size_t ComponentMessenger::HashFunc::operator()(const Key& k) const
        {
            return Core::StdUtils::hash(k);
        }

        template<typename ReturnType>
        inline typename ComponentMessenger::CallbackTypes<ReturnType>::Getter ComponentMessenger::getterCallback(
                const Entity* entity, const std::string& id)
        {
            CORE_ASSERT(canGet<ReturnType>(entity, id), "Unregistered callback");
            return static_cast<GetterCallback <ReturnType>*>(m_entityGetLists[entity][Key(id, std::type_index(
                    typeid(ReturnType)))].get())->m_cb;
        }

        template<typename ReturnType>
        inline typename ComponentMessenger::CallbackTypes<ReturnType>::ReadWrite ComponentMessenger::rwCallback(
                const Entity* entity, const std::string& id)
        {
            CORE_ASSERT(canRw<ReturnType>(entity, id), "Unregistered callback");
            return static_cast<RwCallback <ReturnType>*>(m_entityRwLists[entity][Key(id, std::type_index(
                    typeid(ReturnType)))].get())->m_cb;
        }

        template<typename ReturnType>
        inline typename ComponentMessenger::CallbackTypes<ReturnType>::Setter ComponentMessenger::setterCallback(
                const Entity* entity, const std::string& id)
        {
            CORE_ASSERT(canSet<ReturnType>(entity, id), "Unregistered callback");
            return static_cast<SetterCallback <ReturnType>*>(m_entitySetLists[entity][Key(id, std::type_index(
                    typeid(ReturnType)))].get())->m_cb;
        }

        template<typename ReturnType>
        inline const ReturnType& ComponentMessenger::get(const Entity* entity, const std::string& id)
        {
            return CallbackTypes<ReturnType>::getHelper(getterCallback<ReturnType>(entity, id));
        }
/*
        template<typename ReturnType>
        inline void ComponentMessenger::set(const Entity* entity, const std::string& id, const ReturnType& x)
        {
            return setterCallback<ReturnType>(entity, id)(x);
        }
*/
        template<typename ReturnType>
        inline bool ComponentMessenger::canGet(const Entity* entity, const std::string& id)
        {
            // Attempt to find the given entity list.
            const auto& entityListPos = m_entityGetLists.find(entity);
            CORE_ASSERT(entityListPos != m_entityGetLists.end(), " Entity has no registered component");

            Key key(id, std::type_index(typeid(ReturnType)));
            const CallbackMap& entityList = entityListPos->second;

            // Check if there are components exporting the given type,
            // so let's try to find if there is one with the requested id.
            const auto& callbackEntry = entityList.find(key);
            const bool found = (callbackEntry != entityList.end());
            return found;
        }

        template<typename ReturnType>
        inline bool ComponentMessenger::canSet(const Entity* entity, const std::string& id)
        {
            // Attempt to find the given entity list.
            const auto& entityListPos = m_entitySetLists.find(entity);
            CORE_ASSERT(entityListPos != m_entitySetLists.end(), " Entity has no registered component");

            Key key(id, std::type_index(typeid(ReturnType)));
            const CallbackMap& entityList = entityListPos->second;

            // Check if there are components exporting the given type,
            // so let's try to find if there is one with the requested id.
            const auto& callbackEntry = entityList.find(key);
            const bool found = (callbackEntry != entityList.end());
            return found;
        }

        template<typename ReturnType>
        inline bool ComponentMessenger::canRw(const Entity* entity, const std::string& id)
        {
            // Attempt to find the given entity list.
            const auto& entityListPos = m_entityRwLists.find(entity);
            CORE_ASSERT(entityListPos != m_entityRwLists.end(), " Entity has no registered component");

            Key key(id, std::type_index(typeid(ReturnType)));
            const CallbackMap& entityList = entityListPos->second;

            // Check if there are components exporting the given type,
            // so let's try to find if there is one with the requested id.
            const auto& callbackEntry = entityList.find(key);
            const bool found = (callbackEntry != entityList.end());
            return found;
        }

        template<typename ReturnType>
        inline void ComponentMessenger::registerOutput(const Entity* entity, Component* comp, const std::string& id,
                                                const typename CallbackTypes<ReturnType>::Getter& cb)
        {
            CORE_ASSERT(entity && comp->getEntity() == entity, "Component not added to entity");
            // Will insert a new entity entry if it doesn't exist.
            CallbackMap& entityList = m_entityGetLists[entity];

            Key key(id, std::type_index(typeid(ReturnType)));
            CORE_ASSERT(entityList.find(key) == entityList.end(), "Output function already registered");

            GetterCallback <ReturnType>* getter = new GetterCallback<ReturnType>();
            getter->m_cb = cb;
            entityList[key].reset(getter);
        }

        template<typename ReturnType>
        inline void ComponentMessenger::registerReadWrite(const Entity* entity, Component* comp, const std::string& id,
                                                   const typename CallbackTypes<ReturnType>::ReadWrite& cb)
        {
            CORE_ASSERT(entity && comp->getEntity() == entity, "Component not added to entity");
            // Will insert a new entity entry if it doesn't exist.
            CallbackMap& entityList = m_entityRwLists[entity];

            Key key(id, std::type_index(typeid(ReturnType)));
            CORE_ASSERT(entityList.find(key) == entityList.end(), "Rw function already registered");

            RwCallback <ReturnType>* rw = new RwCallback<ReturnType>();
            rw->m_cb = cb;
            entityList[key].reset(rw);
        }

        template<typename ReturnType>
        inline void ComponentMessenger::registerInput(const Entity* entity, Component* comp, const std::string& id,
                                               const typename CallbackTypes<ReturnType>::Setter& cb)
        {
            CORE_ASSERT(entity && comp->getEntity() == entity, "Component not added to entity");
            // Will insert a new entity entry if it doesn't exist.
            CallbackMap& entityList = m_entitySetLists[entity];

            Key key(id, std::type_index(typeid(ReturnType)));
            CORE_ASSERT(entityList.find(key) == entityList.end(), "Input function already registered");

            SetterCallback <ReturnType>* setter = new SetterCallback<ReturnType>();
            setter->m_cb = cb;
            entityList[key].reset(setter);
        }

    }
}
