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
}
} // namespace Ra
namespace Ra {
namespace Gui {
/// This class interfaces the gizmos with the ui commands.
/// It allows to change the gizmo type when editing an editable transform object
/// Note :  currently the scale gizmo is not implemented so it will just return a null pointer
class RA_GUIBASE_API GizmoManager : public QObject, public GuiBase::TransformEditor
{
    Q_OBJECT

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    enum GizmoType { NONE, TRANSLATION, ROTATION, SCALE };

    explicit GizmoManager( QObject* parent = nullptr );
    ~GizmoManager() = default;

  public:
    /// Receive mouse events and transmit them to the gizmos.
    virtual bool handleMousePressEvent( QMouseEvent* event,
                                        const KeyMappingManager::KeyMappingAction& action );
    virtual bool handleMouseReleaseEvent( QMouseEvent* event,
                                          const KeyMappingManager::KeyMappingAction& action );
    virtual bool handleMouseMoveEvent( QMouseEvent* event,
                                       const KeyMappingManager::KeyMappingAction& action );

  public slots:
    /// Set the object being currently edited
    void setEditable( const Engine::ItemEntry& ent ) override;

    /// Destroy all gizmos
    void cleanup();

    /// Callback when a drawable is picked.
    void handlePickingResult( int drawableId );

    /// Change mode from local axis to global
    void setLocal( bool useLocal );

    /// Change gizmo type (rotation or translation)
    void changeGizmoType( GizmoType type );

    /// Retrieve the transform from the editable and update the gizmos.
    void updateValues() override;

  private:
    // Helper method to change the current gizmo
    void updateGizmo();

    // Returs the current gizmo
    Gizmo* currentGizmo();

  private:
    std::array<std::unique_ptr<Gizmo>, 3> m_gizmos; //! Owning pointers to the gizmos
    GizmoType m_currentGizmoType;                   //! Type of the gizmo
    Gizmo::Mode m_mode;                             //! Local/global axis mode.
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_GIZMO_MANAGER_HPP_
