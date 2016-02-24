#ifndef RADIUMENGINE_ROTATION_EDITOR_HPP_
#define RADIUMENGINE_ROTATION_EDITOR_HPP_

#include <QWidget>
#include <ui_RotationEditor.h>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Gui
    {
        /// A small Qt Widget to edit a rotation with Euler angles (with matched sliders and spin boxes).
        class RotationEditor : public QWidget, private Ui::RotationEditor
        {
            Q_OBJECT
        public:
            RotationEditor( uint id, QString title, bool editable, QWidget* parent = nullptr )
                : QWidget( parent ),
                  m_id( id ),
                  m_relativeAxis( -1 )
            {
                setupUi( this );
                m_groupBox->setTitle( title );

                m_relSpinBoxes[0] = m_x_rel;
                m_relSpinBoxes[1] = m_y_rel;
                m_relSpinBoxes[2] = m_z_rel;
                m_relSliders[0] = m_slider_x_rel;
                m_relSliders[1] = m_slider_y_rel;
                m_relSliders[2] = m_slider_z_rel;

                // A static cast is needed to get the right function pointer.
                // This typedefs makes the declaration easier to read.
                typedef void (QDoubleSpinBox::*spinPtr)(double);
                typedef void (QSlider::*slidePtr)(int);

                connect( m_x, static_cast<spinPtr>(&QDoubleSpinBox::valueChanged), this, &RotationEditor::onValueChangedAbsSpin);
                connect( m_y, static_cast<spinPtr>(&QDoubleSpinBox::valueChanged), this, &RotationEditor::onValueChangedAbsSpin);
                connect( m_z, static_cast<spinPtr>(&QDoubleSpinBox::valueChanged), this, &RotationEditor::onValueChangedAbsSpin);

                connect( m_slider_x, static_cast<slidePtr>(&QSlider::valueChanged), this, &RotationEditor::onValueChangedAbsSlide);
                connect( m_slider_y, static_cast<slidePtr>(&QSlider::valueChanged), this, &RotationEditor::onValueChangedAbsSlide);
                connect( m_slider_z, static_cast<slidePtr>(&QSlider::valueChanged), this, &RotationEditor::onValueChangedAbsSlide);

                connect( m_x_rel, static_cast<spinPtr>(&QDoubleSpinBox::valueChanged), this, &RotationEditor::onValueChangedRelSpinX);
                connect( m_y_rel, static_cast<spinPtr>(&QDoubleSpinBox::valueChanged), this, &RotationEditor::onValueChangedRelSpinY);
                connect( m_z_rel, static_cast<spinPtr>(&QDoubleSpinBox::valueChanged), this, &RotationEditor::onValueChangedRelSpinZ);

                connect( m_slider_x_rel, static_cast<slidePtr>(&QSlider::valueChanged), this, &RotationEditor::onValueChangedRelSlideX);
                connect( m_slider_y_rel, static_cast<slidePtr>(&QSlider::valueChanged), this, &RotationEditor::onValueChangedRelSlideY);
                connect( m_slider_z_rel, static_cast<slidePtr>(&QSlider::valueChanged), this, &RotationEditor::onValueChangedRelSlideZ);

                m_x->setReadOnly(!editable);
                m_y->setReadOnly(!editable);
                m_z->setReadOnly(!editable);
                m_x_rel->setReadOnly(!editable);
                m_y_rel->setReadOnly(!editable);
                m_z_rel->setReadOnly(!editable);

                m_slider_x->setEnabled(editable);
                m_slider_y->setEnabled(editable);
                m_slider_z->setEnabled(editable);
                m_slider_x_rel->setEnabled(editable);
                m_slider_y_rel->setEnabled(editable);
                m_slider_z_rel->setEnabled(editable);

            }

            /// Manually set a new value for the rotation.
            void setValue( const Core::Quaternion& quat )
            {
                Core::Matrix3 m = quat.toRotationMatrix();
                Core::Vector3 ypr = m.eulerAngles( 2, 1, 0 ); //yaw pitch roll.

                // Note on rotation order : the Euler angles are gotten in the order 2,1,0, because
                // I wanted the "yaw" to be between -90 and 90 (and Eigen automatically sets this
                // constraint on the first value). This is why the order is YPr (i.e. ZXY) and not XYZ.
                // Pay attention on how z and x are in reverse order when creating or reading the values
                // from the YPR vector.

                updateAbsSpin( ypr );
                updateAbsSlide( ypr );
                m_relativeAxis = 0;
                resetRel();
            }

        signals:
            /// Emitted when the value has changed through the UI.
            void valueChanged( const Core::Quaternion& newValue, uint id );

        private slots:

            /// Listens to the absolute spin boxes change signals and update the data accordingly.
            void onValueChangedAbsSpin()
            {
                Core::Vector3 ypr( Core::Math::toRadians( Scalar( m_z->value() ) ),
                                   Core::Math::toRadians( Scalar( m_y->value() ) ),
                                   Core::Math::toRadians( Scalar( m_x->value() ) ) );
                resetRel();
                updateAbsSlide( ypr );
                m_startRelTransformYpr = ypr;
                Core::Quaternion quat = Core::AngleAxis( ypr[0], Core::Vector3::UnitZ() )
                                        * Core::AngleAxis( ypr[1], Core::Vector3::UnitY() )
                                        * Core::AngleAxis( ypr[2], Core::Vector3::UnitX() );

                emit( valueChanged( quat, m_id ) );

            };

            /// Listens to the absolute sliders change signals and update the data accordingly.
            void onValueChangedAbsSlide()
            {
                const Scalar x = Scalar( m_slider_x->value() );
                const Scalar y = Scalar( m_slider_y->value() );
                const Scalar z = Scalar( m_slider_z->value() );

                const Core::Vector3 ypr = Core::Vector3( Core::Math::toRadians( z ),
                                                         Core::Math::toRadians( y ),
                                                         Core::Math::toRadians( x ) );
                m_relativeAxis = -1;
                resetRel();
                updateAbsSpin( ypr );
                m_startRelTransformYpr = ypr;

                Core::Quaternion quat = Core::AngleAxis( ypr[0], Core::Vector3::UnitZ() )
                                        * Core::AngleAxis( ypr[1], Core::Vector3::UnitY() )
                                        * Core::AngleAxis( ypr[2], Core::Vector3::UnitX() );

                emit valueChanged( quat, m_id );
            };

            void onValueChangedRelSpinX()
            {
                valueChangedSpinRel( 0 );
            }
            void onValueChangedRelSpinY()
            {
                valueChangedSpinRel( 1 );
            }
            void onValueChangedRelSpinZ()
            {
                valueChangedSpinRel( 2 );
            }
            void onValueChangedRelSlideX()
            {
                valueChangedSlideRel( 0 );
            }
            void onValueChangedRelSlideY()
            {
                valueChangedSlideRel( 1 );
            }
            void onValueChangedRelSlideZ()
            {
                valueChangedSlideRel( 2 );
            }

        private:

            void valueChangedSpinRel( uint axis )
            {
                if ( m_relativeAxis != axis )
                {
                    const Scalar x = Scalar( m_x->value() );
                    const Scalar y = Scalar( m_y->value() );
                    const Scalar z = Scalar( m_z->value() );
                    m_relativeAxis = axis;
                    resetRel();
                    m_startRelTransformYpr = Core::Vector3( Core::Math::toRadians( z ),
                                                            Core::Math::toRadians( y ),
                                                            Core::Math::toRadians( x ) );
                }


                Core::Quaternion quat = Core::AngleAxis( m_startRelTransformYpr[0], Core::Vector3::UnitZ() )
                                        * Core::AngleAxis( m_startRelTransformYpr[1], Core::Vector3::UnitY() )
                                        * Core::AngleAxis( m_startRelTransformYpr[2], Core::Vector3::UnitX() );

                Core::Quaternion rotX( Core::AngleAxis( Core::Math::toRadians( m_relSliders[axis]->value() ),
                                                        Core::Vector3::Unit( axis ) ) );

                quat = quat * rotX;

                const Core::Vector3 newYpr = quat.toRotationMatrix().eulerAngles( 2, 1, 0 );

                updateRelSlide( m_relSliders[axis]->value() );
                updateAbsSpin( newYpr );
                updateAbsSlide( newYpr );

                emit valueChanged( quat, m_id );
            }


            void valueChangedSlideRel( uint axis )
            {
                if ( m_relativeAxis != axis )
                {
                    const Scalar x = Scalar( m_x->value() );
                    const Scalar y = Scalar( m_y->value() );
                    const Scalar z = Scalar( m_z->value() );
                    m_relativeAxis = axis;
                    resetRel();
                    m_startRelTransformYpr = Core::Vector3( Core::Math::toRadians( z ),
                                                            Core::Math::toRadians( y ),
                                                            Core::Math::toRadians( x ) );
                }


                Core::Quaternion quat = Core::AngleAxis( m_startRelTransformYpr[0], Core::Vector3::UnitZ() )
                                        * Core::AngleAxis( m_startRelTransformYpr[1], Core::Vector3::UnitY() )
                                        * Core::AngleAxis( m_startRelTransformYpr[2], Core::Vector3::UnitX() );

                Core::Quaternion rotX( Core::AngleAxis( Core::Math::toRadians( Scalar( m_relSliders[axis]->value() ) ),
                                                        Core::Vector3::Unit( axis ) ) );

                quat = quat *  rotX;

                const Core::Vector3 newYpr = quat.toRotationMatrix().eulerAngles( 2, 1, 0 );

                updateRelSpin( double( m_relSliders[axis]->value() ) );
                updateAbsSpin( newYpr );
                updateAbsSlide( newYpr );

                emit valueChanged( quat, m_id );

            }

            /// Update the spin boxes from a YPR vector.
            void updateAbsSpin( const Core::Vector3& ypr )
            {
                // We disable signals to avoid the spin boxes firing a new "valueChanged()" signal
                // which would create an infinite loop.
                m_x->blockSignals( true );
                m_y->blockSignals( true );
                m_z->blockSignals( true );

                m_x->setValue( Core::Math::toDegrees( ypr.z() ) ); // roll
                m_y->setValue( Core::Math::toDegrees( ypr.y() ) ); // pitch
                m_z->setValue( Core::Math::toDegrees( ypr.x() ) ); // yaw

                m_x->blockSignals( false );
                m_y->blockSignals( false );
                m_z->blockSignals( false );
            }

            /// Update the sliders from a YPR vector.
            void updateAbsSlide( const Core::Vector3& ypr )
            {
                m_slider_x->blockSignals( true );
                m_slider_y->blockSignals( true );
                m_slider_z->blockSignals( true );

                m_slider_x->setValue( int( Core::Math::toDegrees( ypr.z() ) ) ); // roll
                m_slider_y->setValue( int( Core::Math::toDegrees( ypr.y() ) ) ); // pitch
                m_slider_z->setValue( int( Core::Math::toDegrees( ypr.x() ) ) ); // yaw

                m_slider_x->blockSignals( false );
                m_slider_y->blockSignals( false );
                m_slider_z->blockSignals( false );
            }

            void updateRelSlide( double value )
            {
                CORE_ASSERT( m_relativeAxis >= 0 && m_relativeAxis < 3, "Invalid relative axis" );
                QSlider* slider = m_relSliders[m_relativeAxis];
                slider->blockSignals( true );
                slider->setValue( int( value ) );
                slider->blockSignals( false );
            }

            void updateRelSpin( double value )
            {
                CORE_ASSERT( m_relativeAxis >= 0 && m_relativeAxis < 3, "Invalid relative axis" );
                QDoubleSpinBox* box = m_relSpinBoxes[m_relativeAxis] ;
                box->blockSignals( true );
                box->setValue( value );
                box->blockSignals( false );
            }
            void resetRel()
            {
                for ( uint i = 0 ; i < 3; ++i )
                {
                    if ( i != m_relativeAxis )
                    {
                        m_relSpinBoxes[i]->blockSignals( true );
                        m_relSpinBoxes[i]->setValue( 0 );
                        m_relSpinBoxes[i]->blockSignals( false );

                        m_relSliders[i]->blockSignals( true );
                        m_relSliders[i]->setValue( 0 );
                        m_relSliders[i]->blockSignals( false );
                    }
                }
            }
        private:
            uint m_id;

            // Relative rotation helpers
            QDoubleSpinBox* m_relSpinBoxes[3];
            QSlider* m_relSliders[3];

            // Relative rotation state variables.
            Core::Vector3 m_startRelTransformYpr;
            int m_relativeAxis;
        };
    }
}
#endif// RADIUMENGINE_ROTATION_EDITOR_HPP_
