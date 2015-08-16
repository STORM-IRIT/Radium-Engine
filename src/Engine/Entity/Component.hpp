#ifndef RADIUMENGINE_COMPONENT_HPP
#define RADIUMENGINE_COMPONENT_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/IndexedObject.hpp>

#include <Engine/Entity/EditableProperty.hpp>

namespace Ra
{
    namespace Engine
    {
        class System;
        class Entity;
        class RenderObjectManager;
    }
}

namespace Ra
{
    namespace Engine
    {

        /**
         * @brief A component is an element that can be updated by a system.
         * It is also linked to some other components in an entity.
         * Each component share a transform through their entity.
         */
        class RA_API Component
        {
        public:
            /// CONSTRUCTOR
            Component( const std::string& name );

            // FIXME (Charly) : Should destructor call something like
            //                  System::removeComponent(this) ?
            /// DESTRUCTOR
            virtual ~Component();

            /**
            * @brief Pure virtual method to be overridden by any component.
            * When this method is called you are guaranteed to know your system and your entity
            */
            virtual void initialize() = 0;
            /**
             * @brief Set entity the component is part of.
             * This method is called by the entity.
             * @param entity The entity the component is part of.
             */
            virtual void setEntity( Entity* entity );

            virtual void setRenderObjectManager( RenderObjectManager* renderObjectManager );

            virtual Entity* getEntity() const;

            virtual const std::string& getName() const;

            // 
            // Properties management.
            // 

            /// Get a list of all editable properties.
            void getPropertyList(std::vector<EditableProperty>& propsOut) const {};
            
            /// Tell the component to take the new property value into account.
            void setProperty(const EditableProperty& newProp) {};

        protected:
            std::string m_name;

            Entity* m_entity;

            RenderObjectManager* m_renderObjectManager;

        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Entity/Component.inl>

#endif // RADIUMENGINE_COMPONENT_HPP
