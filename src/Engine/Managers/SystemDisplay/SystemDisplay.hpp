#ifndef RADIUMENGINE_DEBUG_DISPLAY_HPP_
#define RADIUMENGINE_DEBUG_DISPLAY_HPP_

#include <Engine/RaEngine.hpp>

/// This file's main purpose is to declare a set of macros to allow drawing
/// primitives from everywhere in the code, for debug purposes.
/// These macros can be completely disabled by #defining the following constant :

// define RA_DISABLE_DEBUG_DISPLAY

#include <Core/Utils/Singleton.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>
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

            /// Access render object through RO manager
            RenderObject* getRenderObject( Core::Index idx );
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
// FIXME(Charly): We should find a better
/// Macros for debug drawing. All coordinates are in world space.
#define RA_DISPLAY_POINT( p, color, scale )                                             \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Point(                                              \
            Ra::Engine::SystemEntity::dbgCmp(), p, color, scale ))

#define RA_DISPLAY_VECTOR( p, v, color )                                                \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Vector(                                             \
            Ra::Engine::SystemEntity::dbgCmp(), p, v, color ) )

#define RA_DISPLAY_RAY( r, color )                                                      \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Ray(                                                \
            Ra::Engine::SystemEntity::dbgCmp(), r, color ))

#define RA_DISPLAY_RAY_LIFETIME( r, color, lifetime )                                   \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Ray(                                                \
            Ra::Engine::SystemEntity::dbgCmp(), r, color, lifetime ))



#define RA_DISPLAY_CIRCLE( c, n, r, color )                                             \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Circle(                                             \
            Ra::Engine::SystemEntity::dbgCmp(), c, n, r, 20, color ) )

#define RA_DISPLAY_TRIANGLE( a, b, c, color )                                           \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Triangle(                                           \
            Ra::Engine::SystemEntity::dbgCmp(), a, b, c, color ) )

#define RA_DISPLAY_NORMAL( p, n, color, scale )                                         \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Normal(                                             \
            Ra::Engine::SystemEntity::dbgCmp(), p, n, color, scale ) )

#define RA_DISPLAY_FRAME( t, scale )                                                    \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Frame(                                              \
            Ra::Engine::SystemEntity::dbgCmp(), t, scale ) )

#define RA_DISPLAY_AABB( a, color )                                                     \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::AABB(                                               \
            Ra::Engine::SystemEntity::dbgCmp(), a, color ) )

#define RA_DISPLAY_OBB( a, color )                                                      \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::OBB(                                                \
            Ra::Engine::SystemEntity::dbgCmp(), a, color ) )

#define RA_DISPLAY_SPHERE( c, r, color )                                                \
    Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                                \
        Ra::Engine::DrawPrimitives::Sphere(                                             \
            Ra::Engine::SystemEntity::dbgCmp(), c, r, color ) )

/// RO update mechanics
#define RA_UPDATE_POINT( idx, p, color, scale )                                         \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Point( ro, p, color, scale );                           \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_VECTOR( idx, p, v, color )                                            \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Vector( ro, p, v, color );                              \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_RAY( idx, r, color )                                                  \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Ray( ro, r, color );                                    \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_TRIANGLE( idx, a, b, c, color )                                       \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Triangle( ro, a, b, c, color );                         \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_CIRCLE( idx, c, n, r, color )                                         \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Circle( ro, c, n, r, color );                           \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_NORMAL( idx, p, n, color, scale )                                     \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Normal( ro, p, n, color, scale );                       \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_FRAME( idx, t, scale )                                                \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::Frame( ro, t, scale );                                  \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_AABB( idx, a, color )                                                 \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::AABB( ro, a, color );                                   \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#define RA_UPDATE_OBB( idx, a, color )                                                  \
{                                                                                       \
    RenderObject* ro = Ra::Engine::SystemEntity::dbgCmp()->updateRenderObject( idx );   \
    Ra::Engine::DrawPrimitives::OBB( ro, a, color );                                    \
    Ra::Engine::SystemEntity::dbgCmp()->doneUpdatingRenderObject( idx );                \
}

#else // if debug display is disabled

#define RA_DISPLAY_POINT( p, color, scale )     // Nothing
#define RA_DISPLAY_VECTOR( p, v, color )        // ...
#define RA_DISPLAY_RAY( r, color )              // ...
#define RA_DISPLAY_TRIANGLE( a, b, c, color )   // ...
#define RA_DISPLAY_NORMAL( p, n, color, scale ) // ...
#define RA_DISPLAY_FRAME( t, scale )            // ...

#endif //!defined DISABLED_DEBUG_DISPLAY

#endif //RADIUMENGINE_DEBUG_DISPLAY_HPP_
