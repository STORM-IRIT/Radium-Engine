#ifndef RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#define RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_
#include <QWidget>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Engine/Entity/EditableProperty.hpp>

class QLayout;

namespace Ra
{
    namespace Gui
    {

        /// The specialized tab to edit the transform of an object.
        class TransformEditorWidget : public QWidget
        {
            Q_OBJECT
        public:
            TransformEditorWidget( QWidget* parent = nullptr );
            ~TransformEditorWidget();

        public slots:
            /// Update the displays from the current state of the editable properties.
            /// This should be called at every frame if the watched object has been updated.
            void updateValues();

            /// Change the object being edited. To clear the UI (e.g. if no object is selected)
            /// you can pass nullptr as the editable.
            void setEditable( Engine::EditableInterface* edit );

        private slots:
            // Called internally by the child widgets when their value change.
            void onChangedPosition( uint id, const Core::Vector3& v );
            void onChangedRotation( uint id, const Core::Quaternion& q );

        private:
            /// Object being edited.
            Engine::EditableInterface* m_currentEdit;

            /// Editable property.
            Engine::EditableProperty m_transform;

            /// Layout of the widgets
            QLayout* m_layout;

            /// Vector of edition widgets, one for each property.
            /// If the corresponding editing is not supported, the widget will be nullptr;
            std::vector<QWidget*> m_widgets;
        };
    }
}

#endif //RADIUMENGINE_ENTITY_PROPERTY_WIDGET_HPP_