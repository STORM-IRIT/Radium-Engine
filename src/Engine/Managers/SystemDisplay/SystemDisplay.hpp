#ifndef RADIUMENGINE_DEBUG_DISPLAY_HPP_
#define RADIUMENGINE_DEBUG_DISPLAY_HPP_

#include <Engine/RaEngine.hpp>

/// This file's main purpose is to declare a set of macros to allow drawing
/// primitives from everywhere in the code, for debug purposes.
/// These macros can be completely disabled by #defining the following constant :

// define RA_DISABLE_DEBUG_DISPLAY

#include <Core/Utils/Color.hpp>
#include <Core/Utils/Singleton.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>

#ifndef RA_DISABLE_DEBUG_DISPLAY
#    include <Engine/Renderer/RenderObject/Primitives/DrawPrimitives.hpp>
#endif

namespace Ra {
namespace Engine {
#ifndef RA_DISABLE_DEBUG_DISPLAY

/// Component for debug drawing. @see SystemEntity.
class RA_ENGINE_API DebugComponent : public Component
{
  public:
    explicit DebugComponent( Entity* entity ) : Component( "Debug", entity ) {}

    void initialize() override {};

    /// Access render object through RO manager
    RenderObject* getRenderObject( Core::Utils::Index idx );
};

#endif

/// Component for UI drawing. @see SystemEntity.
class RA_ENGINE_API UiComponent : public Component
{
  public:
    explicit UiComponent( Entity* entity ) : Component( "UI", entity ) {}

    void initialize() override {}
};

/// This entity allows to add UI and debug drawables from everywhere in the code.
/// It should have only one component and its transform should not change.
class RA_ENGINE_API SystemEntity : public Entity
{
    RA_SINGLETON_INTERFACE( SystemEntity );

  public:
    SystemEntity();

    ~SystemEntity() override = default;

#ifndef RA_DISABLE_DEBUG_DISPLAY
    /// Access the debug component
    static DebugComponent* dbgCmp();

#endif
    /// Access the UI Component
    static UiComponent* uiCmp();
};

} // namespace Engine
} // namespace Ra

#ifndef RA_DISABLE_DEBUG_DISPLAY
/// Macros for debug drawing. All coordinates are in world space.
#    define RA_DISPLAY_POINT( p, color, scale )              \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Point( p, color, scale ) ) )

#    define RA_DISPLAY_VECTOR( p, v, color )                 \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Vector( p, v, color ) ) )

#    define RA_DISPLAY_RAY( r, color )                                                 \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                           \
            Ra::Engine::DrawPrimitives::Primitive( Ra::Engine::SystemEntity::dbgCmp(), \
                                                   Ra::Engine::DrawPrimitives::Ray( r, color ) ) )

#    define RA_DISPLAY_CIRCLE( c, n, r, color )              \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Circle( c, n, r, 20, color ) ) )

#    define RA_DISPLAY_TRIANGLE( a, b, c, color )            \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Triangle( a, b, c, color ) ) )

#    define RA_DISPLAY_NORMAL( p, n, color, scale )          \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Normal( p, n, color, scale ) ) )

#    define RA_DISPLAY_FRAME( t, scale )                     \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Frame( t, scale ) ) )

#    define RA_DISPLAY_AABB( a, color )                      \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::AABB( a, color ) ) )

#    define RA_DISPLAY_OBB( a, color )                                                 \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject(                           \
            Ra::Engine::DrawPrimitives::Primitive( Ra::Engine::SystemEntity::dbgCmp(), \
                                                   Ra::Engine::DrawPrimitives::OBB( a, color ) ) )

#    define RA_DISPLAY_SPHERE( c, r, color )                 \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Sphere( c, r, color ) ) )

#    define RA_DISPLAY_CAPSULE( p1, p2, r, color )           \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Capsule( p1, p2, r, color ) ) )

#    define RA_DISPLAY_LINE( a, b, color )                   \
        Ra::Engine::SystemEntity::dbgCmp()->addRenderObject( \
            Ra::Engine::DrawPrimitives::Primitive(           \
                Ra::Engine::SystemEntity::dbgCmp(),          \
                Ra::Engine::DrawPrimitives::Line( a, b, color ) ) )

#else // if debug display is disabled

#    define RA_DISPLAY_POINT( p, color, scale )     // Nothing
#    define RA_DISPLAY_VECTOR( p, v, color )        // ...
#    define RA_DISPLAY_RAY( r, color )              // ...
#    define RA_DISPLAY_CIRCLE( c, n, r, color )     // ...
#    define RA_DISPLAY_TRIANGLE( a, b, c, color )   // ...
#    define RA_DISPLAY_NORMAL( p, n, color, scale ) // ...
#    define RA_DISPLAY_FRAME( t, scale )            // ...
#    define RA_DISPLAY_AABB( a, color )             // ...
#    define RA_DISPLAY_OBB( a, color )              // ...
#    define RA_DISPLAY_SPHERE( c, r, color )        // ...
#    define RA_DISPLAY_CAPSULE( p1, p2, r, color )  // ...
#    define RA_DISPLAY_LINE( a, b, color )          // ...

#endif //! defined DISABLED_DEBUG_DISPLAY

#endif // RADIUMENGINE_DEBUG_DISPLAY_HPP_
