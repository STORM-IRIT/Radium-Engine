#ifndef RADIUMENGINE_VECTOR_EDITOR_HPP_
#define RADIUMENGINE_VECTOR_EDITOR_HPP_

#include <QWidget>
#include <ui_VectorEditor.h>
namespace Ra
{
    namespace Gui
    {

        /// A small Qt widget to edit a vector3 value.
        class VectorEditor : public QWidget, private Ui::VectorEditor
        {
            Q_OBJECT
        public:
            VectorEditor( uint id, QString title, bool editable = true, QWidget* parent = nullptr ) : QWidget( parent ), m_id( id )
            {
                setupUi( this );
                m_groupBox->setTitle( title );
                connect( m_x, SIGNAL( valueChanged( double ) ), this, SLOT( onValueChangedInternal() ) );
                connect( m_y, SIGNAL( valueChanged( double ) ), this, SLOT( onValueChangedInternal() ) );
                connect( m_z, SIGNAL( valueChanged( double ) ), this, SLOT( onValueChangedInternal() ) );

                m_x->setReadOnly(!editable);
                m_y->setReadOnly(!editable);
                m_z->setReadOnly(!editable);
            }

            /// Manually set a new value for the vector.
            void setValue( const Core::Vector3& vec )
            {
                m_x->setValue( vec.x() );
                m_y->setValue( vec.y() );
                m_z->setValue( vec.z() );
            }

        signals:
            /// Emitted when the value changes through the UI.
            void valueChanged( uint id, const Core::Vector3& newValue );

        private slots:

            /// Listens to the spin box changes and fires the signal.
            void onValueChangedInternal()
            {
                Core::Vector3 v( m_x->value(), m_y->value(), m_z->value() );
                emit( valueChanged( m_id, v ) );
            };
        private:
            uint m_id;
        };
    }
}
#endif// RADIUMENGINE_VECTOR_EDITOR_HPP_
