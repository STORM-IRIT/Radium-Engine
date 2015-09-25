#ifndef RADIUMENGINE_DEBUG_DISPLAY_HPP_
#define RADIUMENGINE_DEBUG_DISPLAY_HPP_

#include <Engine/RaEngine.hpp>

/// This file's main purpose is to declare a set of macros to allow drawing
/// primitives from everywhere in the code, for debug purposes.
/// These macros can be completely disabled by #defining the following constant :

// define RA_DISABLE_DEBUG_DISPLAY

#include <Core/Utils/Singleton.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>

namespace Ra
{
    namespace Engine
    {
#ifndef RA_DISABLE_DEBUG_DISPLAY

        /// Component for debug drawing. @see SystemEntity.
        class RA_ENGINE_API DebugComponent : public Component
        {
        public:
            DebugComponent() : Component("Debug") { }

            void initialize() override { }
        };

#endif

        /// Component for UI drawing. @see SystemEntity.
        class RA_ENGINE_API UiComponent : public Component
        {
        public:
            UiComponent() : Component("UI") { }

            void initialize() override { }
        };


        /// This entity allows to add UI and debug drawables from everywhere in the code.
        /// It should have only one component and its transform should not change.
        class RA_ENGINE_API SystemEntity : public Entity
        {
        RA_SINGLETON_INTERFACE(SystemEntity);

        public:
            SystemEntity();

            virtual ~SystemEntity() { };

#ifndef RA_DISABLE_DEBUG_DISPLAY

            /// Access the debug component
            static DebugComponent* dbgCmp();

#endif

            /// Access the UI Component
            static UiComponent* uiCmp();

            // Override Editable interface to disable property editing
            virtual void getProperties(Core::AlignedStdVector<EditableProperty>& entityPropsOut) const override { }

            virtual void setProperty(const EditableProperty& prop) override { }

        };

    }
}


#ifndef RA_DISABLE_DEBUG_DISPLAY
/// Macros for debug drawing. All coordinates are in world space.
#define RA_DISPLAY_POINT( p, color, scale )     Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Point(Ra::Engine::SystemEntity::dbgCmp(), p, color, scale ))
#define RA_DISPLAY_VECTOR( p, v, color )        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Vector(Ra::Engine::SystemEntity::dbgCmp(), p, v, color ))
#define RA_DISPLAY_RAY( r, color )              Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Ray(Ra::Engine::SystemEntity::dbgCmp(), r, color ))
#define RA_DISPLAY_CIRCLE( c, n, r, color )     Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Circle(Ra::Engine::SystemEntity::dbgCmp(), c, n, r, 20, color ))
#define RA_DISPLAY_TRIANGLE( a, b, c, color )   Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Triangle(Ra::Engine::SystemEntity::dbgCmp(), a, b, c, color ))
#define RA_DISPLAY_NORMAL( p, n, color, scale ) Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Normal(Ra::Engine::SystemEntity::dbgCmp(), p, n, color, scale ))
#define RA_DISPLAY_FRAME( t, scale )            Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( Ra::Engine::DrawPrimitives::Frame(Ra::Engine::SystemEntity::dbgCmp(), t, scale ))

#else // if debug display is disabled

#define RA_DISPLAY_POINT( p, color, scale )     // Nothing
#define RA_DISPLAY_VECTOR( p, v, color )        // ...
#define RA_DISPLAY_RAY( r, color )              // ...
#define RA_DISPLAY_TRIANGLE( a, b, c, color )   // ...
#define RA_DISPLAY_NORMAL( p, n, color, scale ) // ...
#define RA_DISPLAY_FRAME( t, scale )            // ...

#endif //!defined DISABLED_DEBUG_DISPLAY

#endif //RADIUMENGINE_DEBUG_DISPLAY_HPP_
