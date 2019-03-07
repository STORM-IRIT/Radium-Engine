#ifndef RADIUMENGINE_ROTATION_EDITOR_HPP_
#define RADIUMENGINE_ROTATION_EDITOR_HPP_

#include <Core/Math/LinearAlgebra.hpp>
#include <QWidget>
#include <ui_RotationEditor.h>

namespace Ra {
namespace Gui {

/**
 * A small Qt Widget to edit a rotation with Euler angles
 * (with matched sliders and spin boxes).
 */
class RotationEditor : public QWidget, private Ui::RotationEditor {
    Q_OBJECT
  public:
    RotationEditor( uint id, QString title, bool editable, QWidget* parent = nullptr );

    /**
     * Manually set a new value for the rotation.
     */
    void setValue( const Core::Quaternion& quat );

  signals:
    /**
     * Emitted when the value has changed through the UI.
     */
    void valueChanged( const Core::Quaternion& newValue, uint id );

  private slots:
    /**
     * Slot for the user changing the absolute rotation values.
     */
    void onValueChangedAbsSpin();

    /**
     * Slot for the user changing the absolute rotation sliders.
     */
    void onValueChangedAbsSlide();

    /**
     * Slot for the user changing the relative rotation values.
     */
    void onValueChangedSpinRel( uint axis );

    /**
     * Slot for the user changing the relative rotation sliders.
     */
    void onValueChangedSlideRel( uint axis );

  private:
    /**
     * Update the absolute values from a YPR vector.
     */
    void updateAbsSpin( const Core::Vector3& ypr );

    /**
     * Update the absolute sliders from a YPR vector.
     */
    void updateAbsSlide( const Core::Vector3& ypr );

    /**
     * Update the relative values from \p value.
     */
    void updateRelSlide( double value );

    /**
     * Update the relative sliders from \p value.
     */
    void updateRelSpin( double value );

    /**
     * Reset the relative values and sliders to zero.
     */
    void resetRel();

  private:
    /// The index of the editor.
    uint m_id;

    /// Relative rotation values.
    QDoubleSpinBox* m_relSpinBoxes[3];

    /// Relative rotation sliders.
    QSlider* m_relSliders[3];

    /// Relative rotation transformation angles.
    Core::Vector3 m_startRelTransformYpr;

    /// Current relative rotation axis.
    int m_relativeAxis;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_ROTATION_EDITOR_HPP_
