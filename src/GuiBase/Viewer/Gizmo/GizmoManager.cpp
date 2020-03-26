#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <GuiBase/Utils/Keyboard.hpp>
#include <GuiBase/Viewer/CameraManipulator.hpp>
#include <GuiBase/Viewer/Gizmo/GizmoManager.hpp>
#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>
#include <GuiBase/Viewer/Gizmo/ScaleGizmo.hpp>
#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>

namespace Ra {
namespace Gui {

///\todo If a macro expert could write a recursive marco to have something like
/// KM_DEFINE_STATICS(GizmoManager, KeyMappingGizmo)
#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction GizmoManager::XX;
KeyMappingGizmo
#undef KMA_VALUE

    void
    GizmoManager::configureKeyMapping_impl() {
    m_keyMappingContext = Gui::KeyMappingManager::getInstance()->getContext( "GizmoContext" );
    if ( m_keyMappingContext.isInvalid() )
    {
        LOG( Ra::Core::Utils::logINFO )
            << "GizmoContext not defined (maybe the configuration file do not contains it)";
        LOG( Ra::Core::Utils::logERROR ) << "GizmoContext all keymapping invalide !";
        return;
    }
#define KMA_VALUE( XX ) \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingGizmo
#undef KMA_VALUE
}

bool GizmoManager::isValidAction( const Gui::KeyMappingManager::KeyMappingAction& action ) const {

    if ( action.isInvalid() ) return false;
    bool res = false;

#define KMA_VALUE( XX ) res = ( XX == action ) || res;
    KeyMappingGizmo
#undef KMA_VALUE
        return res;
}

/*
 * FIXME : Mathias -- Creating gizmos by default is not a good idea.
 * implies that all applications developped on top of the engine will have them.
 * This is not a good idea. Applications must be able to define and
 * create their own gizmos
 *
 * \see issue #194
 */
GizmoManager::GizmoManager( QObject* parent ) :
    QObject( parent ), m_currentGizmoType( NONE ), m_mode( Gizmo::GLOBAL ) {
    m_gizmos[0].reset( new TranslateGizmo(
        Engine::SystemEntity::uiCmp(), Ra::Core::Transform::Identity(), m_transform, m_mode ) );
    m_gizmos[1].reset( new RotateGizmo(
        Engine::SystemEntity::uiCmp(), Ra::Core::Transform::Identity(), m_transform, m_mode ) );
    m_gizmos[2].reset( new ScaleGizmo(
        Engine::SystemEntity::uiCmp(), Ra::Core::Transform::Identity(), m_transform, m_mode ) );
    for ( auto& g : m_gizmos )
    {
        if ( g ) { g->show( false ); }
    }
}

void GizmoManager::setEditable( const Engine::ItemEntry& ent ) {
    TransformEditor::setEditable( ent );
    updateGizmo();
}

void GizmoManager::updateGizmo() {
    for ( auto& g : m_gizmos )
    {
        if ( g ) { g->show( false ); }
    }

    if ( canEdit() )
    {
        Core::Transform worldTransform = getWorldTransform();
        auto g                         = currentGizmo();
        if ( g )
        {
            g->updateTransform( m_mode, worldTransform, m_transform );
            g->show( true );
        }
    }
}

void GizmoManager::setLocal( bool useLocal ) {
    m_mode = useLocal ? Gizmo::LOCAL : Gizmo::GLOBAL;
    updateGizmo();
}

void GizmoManager::changeGizmoType( GizmoManager::GizmoType type ) {
    m_currentGizmoType = type;
    updateGizmo();
}

void GizmoManager::updateValues() {
    if ( canEdit() )
    {
        getTransform();
        if ( currentGizmo() )
        { currentGizmo()->updateTransform( m_mode, getWorldTransform(), m_transform ); }
    }
}

bool GizmoManager::handleMousePressEvent( QMouseEvent* event,
                                          const Qt::MouseButtons& buttons,
                                          const Qt::KeyboardModifiers& modifiers,
                                          int key ) {

    if ( !canEdit() || m_currentGizmoType == NONE || !currentGizmo()->isSelected() )
    { return false; }
    auto action = KeyMappingManager::getInstance()->getAction(
        m_keyMappingContext, buttons, modifiers, key, false );

    if ( !( isValidAction( action ) ) ) { return false; }

    const Engine::Camera& cam = CameraManipulator::getCameraFromViewer( parent() );
    currentGizmo()->setInitialState( cam,
                                     Core::Vector2( Scalar( event->x() ), Scalar( event->y() ) ) );
    return true;
}

bool GizmoManager::handleMouseReleaseEvent( QMouseEvent* /*event*/ ) {
    if ( currentGizmo() ) { currentGizmo()->selectConstraint( -1 ); }
    return ( currentGizmo() != nullptr );
}

bool GizmoManager::handleMouseMoveEvent( QMouseEvent* event,
                                         const Qt::MouseButtons& buttons,
                                         const Qt::KeyboardModifiers& modifiers,
                                         int key ) {
    ///\todo what about if someone start a motion with a key, and then release it while moving the
    /// mouse ?
    auto action = KeyMappingManager::getInstance()->getAction(
        m_keyMappingContext, buttons, modifiers, key, false );

    if ( m_currentGizmoType != NONE && canEdit() && isValidAction( action ) && currentGizmo() &&
         currentGizmo()->isSelected() )
    {
        Core::Vector2 currentXY( event->x(), event->y() );
        const Engine::Camera& cam = CameraManipulator::getCameraFromViewer( parent() );
        bool step  = action == GIZMOMANAGER_STEP || action == GIZMOMANAGER_STEP_WHOLE;
        bool whole = action == GIZMOMANAGER_WHOLE || action == GIZMOMANAGER_STEP_WHOLE;

        Core::Transform newTransform = currentGizmo()->mouseMove( cam, currentXY, step, whole );
        setTransform( newTransform );
        updateValues();
        return true;
    }
    return false;
}

void GizmoManager::handlePickingResult( int drawableId ) {
    if ( currentGizmo() ) { currentGizmo()->selectConstraint( drawableId ); }
}

Gizmo* GizmoManager::currentGizmo() {
    return ( m_currentGizmoType == NONE ) ? nullptr : m_gizmos[m_currentGizmoType - 1].get();
}

void GizmoManager::cleanup() {
    for ( auto& g : m_gizmos )
    {
        g.reset( nullptr );
    }
}
} // namespace Gui
} // namespace Ra
