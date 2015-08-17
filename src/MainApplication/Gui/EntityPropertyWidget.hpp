#ifndef RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#define RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#include <QWidget>

#include <Engine/Entity/EditableProperty.hpp>

class QLayout;

namespace Ra {
namespace Gui {

/// The specialized tab to edit properties.
class EntityPropertyWidget : public QWidget
{
    Q_OBJECT
public:
    EntityPropertyWidget(QWidget* parent = nullptr);

public slots:
    /// Update the displays from the current state of the editable properties.
    void updateValues();

public:
    /// Change the object being edited.
    // TODO : Could this be a slot ?
    void setEditable(Engine::EditableInterface* edit);

private slots:
    // Called internally by the child widgets when their value change.
    void onValueChanged( uint id, const Core::Vector3& v );

private:
    /// Object being edited.
    Engine::EditableInterface* m_currentEdit;

    /// Layout of the widgets
    QLayout* m_layout;

    /// Vector of the properties of the object.
    std::vector<Engine::EditableProperty> m_props;

    /// Vector of edition widgets, one for each property.
    /// If the corresponding property cannot be edited, the widget will be nullptr;
    std::vector<QWidget*> m_widgets;
};
}
}

#endif //RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_