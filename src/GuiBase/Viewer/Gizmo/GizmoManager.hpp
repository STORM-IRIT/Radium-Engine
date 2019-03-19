#ifndef RADIUMENGINE_GIZMO_MANAGER_HPP_
#define RADIUMENGINE_GIZMO_MANAGER_HPP_

#include <memory>

#include <GuiBase/TransformEditor/TransformEditor.hpp>
#include <GuiBase/Viewer/Gizmo/Gizmo.hpp>
#include <QMouseEvent>
#include <QObject>

namespace Ra {
namespace Engine {
struct ItemEntry;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Gui {
/**
 * This class interfaces the gizmos with the ui commands.
 * It allows to change the gizmo type when editing an editable transform object.
 */
class RA_GUIBASE_API GizmoManager : public QObject, public GuiBase::TransformEditor {
    Q_OBJECT

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

    /**
     * Available types of Gizmo.
     */
    enum GizmoType { NONE, TRANSLATION, ROTATION, SCALE };

    GizmoManager( QObject* parent = nullptr );

    ~GizmoManager() override;

  public:
    /** \name Handling Mouse Events
     * Receive mouse events and transmit them to the active Gizmo.
     */
    /// \{
    virtual bool handleMousePressEvent( QMouseEvent* event );
    virtual bool handleMouseReleaseEvent( QMouseEvent* event );
    virtual bool handleMouseMoveEvent( QMouseEvent* event );
    /// \}

  public slots:
    /**
     * Set the object being currently edited.
     */
    void setEditable( const Engine::ItemEntry& ent ) override;

    /**
     * Destroy all Gizmos.
     */
    // FIXME: should be private, or even better just implemented in the destructor!
    void cleanup();

    /**
     * Callback when a drawable is picked.
     */
    void handlePickingResult( int drawableId );

    /**
     * Toggle local space mode.
     */
    void setLocal( bool useLocal );

    /**
     * Change Gizmo type.
     */
    void changeGizmoType( GizmoType type );

    /**
     * Retrieve the transform from the editable and update the current Gizmo.
     */
    void updateValues() override;

  private:
    /**
     * Update the current Gizmo drawables.
     */
    void updateGizmo();

    /**
     * Return the current Gizmo.
     */
    Gizmo* currentGizmo();

  private:
    std::array<std::unique_ptr<Gizmo>, 3> m_gizmos; ///< The available gizmos.
    GizmoType m_currentGizmoType;                   ///< Type of the current gizmo.
    Gizmo::Mode m_mode;                             ///< Local/global axis mode.
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
