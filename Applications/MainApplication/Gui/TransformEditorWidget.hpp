#ifndef RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#define RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#include <QWidget>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Gui/VectorEditor.hpp>
#include <GuiBase/TransformEditor/TransformEditor.hpp>

class QLayout;

namespace Ra {
namespace Gui {

/// The specialized tab to edit the transform of an object.
/// For now, only deals with translations.
class TransformEditorWidget : public QWidget, public GuiBase::TransformEditor {
    Q_OBJECT
  public:
    TransformEditorWidget( QWidget* parent = nullptr );
    ~TransformEditorWidget(){};

  public slots:

    /// Change the object being edited.
    void setEditable( const Engine::ItemEntry& ent ) override;

    /// Update the displays from the current state of the editable properties.
    /// This should be called at every frame if the watched object has been updated.
    void updateValues() override;

  private slots:
    /// Replace the translation part of the transform by \p v.
    void onChangedPosition( const Core::Vector3& v, uint id );

    // TODO: not implemented yet, see RotationEditor.
    // void onChangedRotation( const Core::Quaternion& q, uint id );

  private:
    /// Layout of the widgets.
    QLayout* m_layout;

    /// Edition widget for translations.
    VectorEditor* m_translationEditor;
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
