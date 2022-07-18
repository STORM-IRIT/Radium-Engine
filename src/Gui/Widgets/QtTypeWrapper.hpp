#pragma once

#include <QDoubleSpinBox>
#include <QSpinBox>

namespace Ra {
namespace Gui {
namespace Widgets {
/**
 * \brief Associates Qt spinbox type to a given numeric type
 */
namespace QtSpinBox {
template <typename T>
struct getType {
    using Type       = QSpinBox;
    using SignalType = int;
};

template <>
struct getType<float> {
    using Type       = QDoubleSpinBox;
    using SignalType = double;
};

template <>
struct getType<double> {
    using Type       = QDoubleSpinBox;
    using SignalType = double;
};

} // namespace QtSpinBox

} // namespace Widgets
} // namespace Gui
} // namespace Ra
