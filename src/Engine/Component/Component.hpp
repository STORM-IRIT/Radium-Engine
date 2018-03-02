#ifndef RADIUMENGINE_COMPONENT_HPP
#define RADIUMENGINE_COMPONENT_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Ray.hpp>
#include <Core/Index/IndexedObject.hpp>


namespace Ra
{
    namespace Engine
    {
        class System;
        class Entity;
        class RenderObject;
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
        class RA_ENGINE_API Component
        {
        public:
            /// CONSTRUCTOR
            Component( const std::string& name );

            /// DESTRUCTOR
            virtual ~Component();

            /**
            * @brief Pure virtual method to be overridden by any component.
            * When this method is called you are guaranteed that all other startup systems
            * have been loaded.
            */
            virtual void initialize() = 0;
            /**
             * @brief Set entity the component is part of.
             * This method is called by the entity.
             * @param entity The entity the component is part of.
             */
            virtual void setEntity( Entity* entity );

            /// Return the entity the component belongs to
            virtual Entity* getEntity() const;

            /// Return the component's name
            virtual const std::string& getName() const;

            /// Set the system to which the  component belongs.
            virtual void setSystem( System* system );

            /// Returns the system to which the component belongs.
            virtual System* getSystem() const;

            /// Add a new render object to the component. This adds the RO to the manager for drawing.
            virtual Core::Index addRenderObject( RenderObject* renderObject ) final;

            /// Remove the render object from the component.
            virtual void removeRenderObject( Core::Index roIdx ) final;

            /// Perform a ray cast query.
            virtual void rayCastQuery(const Core::Ray& ray) const;

            // Editable transform interface.
            // This allow to edit the data in the component with a render object
            // as a key. An invalid RO index can be passed, meaning no specific RO is
            // queried.

            /// Returns true if a transform can be edited with the render object index given as a key.
            virtual bool canEdit( Core::Index roIdx ) const { return false; }

            /// Get the transform associated with the given RO index key.
            virtual Core::Transform getTransform( Core::Index roIdx ) const { return Core::Transform::Identity();}

            /// Set the new transform associated with the RO index key.
            virtual void setTransform ( Core::Index roIdx, const Core::Transform& transform ) {}

            void notifyRenderObjectExpired( const Core::Index& idx );

        protected:
            /// Shortcut to access the render object manager.
            static RenderObjectManager* getRoMgr();


        public:
            std::vector<Core::Index> m_renderObjects;

        protected:

            std::string m_name;
            Entity* m_entity;
            System* m_system;

        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Component/Component.inl>

#endif // RADIUMENGINE_COMPONENT_HPP
