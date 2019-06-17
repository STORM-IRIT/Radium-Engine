#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <GuiBase/Utils/KeyMappingManager.hpp>
#include <GuiBase/Utils/Keyboard.hpp>
#include <GuiBase/Viewer/CameraInterface.hpp>
#include <GuiBase/Viewer/Gizmo/GizmoManager.hpp>
#include <GuiBase/Viewer/Gizmo/RotateGizmo.hpp>
#include <GuiBase/Viewer/Gizmo/ScaleGizmo.hpp>
#include <GuiBase/Viewer/Gizmo/TranslateGizmo.hpp>

namespace Ra {
namespace Gui {
/*
 * FIXME : Mathias -- Creating gizmos by default is not a good idea.
 * implies that all applications developped on top of the engine will have them.
 * This is not a good idea. Applications mus be able to define and
 * create their their own gizmos
 *
 * \see issue #194
 */
GizmoManager::GizmoManager( QObject* parent ) :
    QObject( parent ),
    m_currentGizmoType( NONE ),
    m_mode( Gizmo::GLOBAL ) {
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
                                          const KeyMappingManager::KeyMappingAction& action ) {

    if ( !canEdit() || m_currentGizmoType == NONE ) { return false; }

    // If we are there it means that we should have a valid gizmo.
    CORE_ASSERT( currentGizmo(), "Gizmo is not there !" );

    const Engine::Camera& cam = CameraInterface::getCameraFromViewer( parent() );
    currentGizmo()->setInitialState( cam,
                                     Core::Vector2( Scalar( event->x() ), Scalar( event->y() ) ) );

    return true;
}

bool GizmoManager::handleMouseReleaseEvent( QMouseEvent* /*event*/,
                                            const KeyMappingManager::KeyMappingAction& action ) {
    if ( currentGizmo() ) { currentGizmo()->selectConstraint( -1 ); }
    return ( currentGizmo() != nullptr );
}

bool GizmoManager::handleMouseMoveEvent( QMouseEvent* event,
                                         const KeyMappingManager::KeyMappingAction& action ) {
    if ( ( action == Gui::KeyMappingManager::GIZMOMANAGER_MANIPULATION ||
           action == Gui::KeyMappingManager::GIZMOMANAGER_STEP ) &&
         currentGizmo() )
    {
        Core::Vector2 currentXY( event->x(), event->y() );
        const Engine::Camera& cam    = CameraInterface::getCameraFromViewer( parent() );
        bool step                    = action == Gui::KeyMappingManager::GIZMOMANAGER_STEP;
        Core::Transform newTransform = currentGizmo()->mouseMove( cam, currentXY, step );
        setTransform( newTransform );
    }
    return ( currentGizmo() != nullptr );
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
