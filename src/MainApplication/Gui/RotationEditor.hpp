#ifndef RADIUMENGINE_ROTATION_EDITOR_HPP_
#define RADIUMENGINE_ROTATION_EDITOR_HPP_

#include <QWidget>
#include <ui_RotationEditor.h>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Gui{
    /// A small Qt Widget to edit a rotation with Euler angles (with matched sliders and spin boxes).
class RotationEditor : public QWidget, private Ui::RotationEditor
{
    Q_OBJECT
public:
    RotationEditor(uint id, QString title, QWidget* parent = nullptr) : QWidget(parent), m_id(id)
    {
        setupUi(this);
        m_groupBox->setTitle(title);

        connect(m_x, SIGNAL(valueChanged(double)), this, SLOT(onValueChangedInternalSpin()));
        connect(m_y, SIGNAL(valueChanged(double)), this, SLOT(onValueChangedInternalSpin()));
        connect(m_z, SIGNAL(valueChanged(double)), this, SLOT(onValueChangedInternalSpin()));

        connect(m_slider_x, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedInternalSlide()));
        connect(m_slider_y, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedInternalSlide()));
        connect(m_slider_z, SIGNAL(valueChanged(int)), this, SLOT(onValueChangedInternalSlide()));
    }

    /// Manually set a new value for the rotation.
    void setValue(const Core::Quaternion& quat)
    {
        Core::Matrix3 m = quat.toRotationMatrix();
        Core::Vector3 ypr = m.eulerAngles(2,1,0); //yaw pitch roll.

        // Note on rotation order : the Euler angles are gotten in the order 2,1,0, because
        // I wanted the "yaw" to be between -90 and 90 (and Eigen automatically sets this
        // constraint on the first value). This is why the order is YPr (i.e. ZXY) and not XYZ.
        // Pay attention on how z and x are in reverse order when creating or reading the values
        // from the YPR vector.

        updateSpin(ypr);
        updateSlide(ypr);
    }

signals:
    /// Emitted when the value has changed through the UI.
    void valueChanged(uint id, const Core::Quaternion& newValue);

private slots:

    /// Listens to the spin boxes change signals and update the data accordingly.
    void onValueChangedInternalSpin()
    {
        Core::Vector3 ypr (Core::Math::toRadians(Scalar(m_z->value())),
                           Core::Math::toRadians(Scalar(m_y->value())),
                           Core::Math::toRadians(Scalar(m_x->value())));
        updateSlide(ypr);
        Core::Quaternion quat = Core::AngleAxis(ypr[0], Core::Vector3::UnitZ())
                                * Core::AngleAxis(ypr[1], Core::Vector3::UnitY())
                                * Core::AngleAxis(ypr[2], Core::Vector3::UnitX());

        emit(valueChanged(m_id,quat));

    };

    /// Listens to the sliders change signals and update the data accordingly.
    void onValueChangedInternalSlide()
    {
        const Scalar x = Scalar(m_slider_x->value());
        const Scalar y = Scalar(m_slider_y->value());
        const Scalar z = Scalar(m_slider_z->value());



        const Core::Vector3 ypr = Core::Vector3( Core::Math::toRadians(z),
                                                 Core::Math::toRadians(y),
                                                 Core::Math::toRadians(x));


        updateSpin(ypr);

        Core::Quaternion quat = Core::AngleAxis(ypr[0], Core::Vector3::UnitZ())
                                * Core::AngleAxis(ypr[1], Core::Vector3::UnitY())
                                * Core::AngleAxis(ypr[2], Core::Vector3::UnitX());

        emit(valueChanged(m_id,quat));
    };

private:
    /// Update the spin boxes from a YPR vector.
    void updateSpin(const Core::Vector3& ypr)
    {
        // We disable signals to avoid the spin boxes firing a new "valueChanged()" signal
        // which would create an infinite loop.
        m_x->blockSignals(true);
        m_y->blockSignals(true);
        m_z->blockSignals(true);

        m_x->setValue(Core::Math::toDegrees(ypr.z())); // roll
        m_y->setValue(Core::Math::toDegrees(ypr.y())); // pitch
        m_z->setValue(Core::Math::toDegrees(ypr.x())); // yaw

        m_x->blockSignals(false);
        m_y->blockSignals(false);
        m_z->blockSignals(false);
    }

    /// Update the sliders from a YPR vector.
    void updateSlide(const Core::Vector3& ypr)
    {
        m_slider_x->blockSignals(true);
        m_slider_y->blockSignals(true);
        m_slider_z->blockSignals(true);

        m_slider_x->setValue(int(Core::Math::toDegrees(ypr.z()))); // roll
        m_slider_y->setValue(int(Core::Math::toDegrees(ypr.y()))); // pitch
        m_slider_z->setValue(int(Core::Math::toDegrees(ypr.x()))); // yaw

        m_slider_x->blockSignals(false);
        m_slider_y->blockSignals(false);
        m_slider_z->blockSignals(false);
    }
private:
    uint m_id;
};
}
}
#endif// RADIUMENGINE_ROTATION_EDITOR_HPP_
