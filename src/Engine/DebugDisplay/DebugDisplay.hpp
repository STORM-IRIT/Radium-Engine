#ifndef RADIUMENGINE_DEBUG_DISPLAY_HPP_
#define RADIUMENGINE_DEBUG_DISPLAY_HPP_

// define RA_DISABLE_DEBUG_DISPLAY
#ifndef RA_DISABLE_DEBUG_DISPLAY

#include <Core/Utils/Singleton.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Renderer/RenderObject/DrawPrimitives.hpp>

namespace Ra
{
    namespace Engine
    {
        class RenderObject;

        class DebugComponent : public Component
        {
        public:
            DebugComponent();
            void initialize() override {}
            void addDebugDrawable( RenderObject* ro );
        private:
            std::vector<Core::Index> m_debugDrawableIndices;
        };

        /// This entity allows to add debug drawables from everywhere in the code.
        /// It should have only one component and its transform should not change.
        class DebugEntity: public Entity, public Core::Singleton<DebugEntity>
        {
            DebugEntity();

            /// Access to its one component.
            DebugComponent* getDebugComponent()  const;

            /// Shortcut to getInstance->getDebugComponent();
            static inline DebugComponent* dbgCmp ()
            {
                return getInstanceRef().getDebugComponent();
            }

            // Override Editable interface to disable property editing
            virtual void getProperties( Core::AlignedStdVector<EditableProperty>& entityPropsOut ) const override {}
            virtual void setProperty( const EditableProperty& prop ) override {}

        };

    }
}

#define RA_DISPLAY_POINT( p, color, scale ) Ra::Engine::DebugEntity::dbgCmp()->addDebugDrawable( Ra::Engine::DrawPrimitives::Point(Ra::Engine::DebugEntity::dbgCmp()), point, color, scale )

#endif //!defined DISABLED_DEBUG_DISPLAY

#endif //RADIUMENGINE_DEBUG_DISPLAY_HPP_
