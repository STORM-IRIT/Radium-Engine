#pragma once
#include <Gui/RaGui.hpp>

#include <QDoubleSpinBox>
#include <QSpinBox>

#include <QWidget>

namespace Ra {
namespace Gui {
namespace Widgets {

/**
 * \brief Validate a given value against user-defined predicates.
 * \tparam T Type of the data to validate
 */
template <typename T>
class SpinValueValidator
{
  public:
    using Predicate = std::function<bool( T )>;
    /// Set the predicate to evaluate
    inline void setPredicate( Predicate p ) { m_p = p; }
    /**
     * \brief Validate a value
     * \param s
     * \return true if the value is valid according to the stored predicate, false otherwise.
     */
    inline bool isValid( T s ) const { return m_p( s ); }

  private:
    Predicate m_p = []( T ) { return true; };
};

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

/**
 * \brief Constrained input spin box.
 * The constraint to apply to any input value is verified using the user-define predicate associated
 * to the object. \tparam T Type of the constrained value to input
 */
template <typename T>
class ConstrainedNumericSpinBox : public QtSpinBox::getType<T>::Type, public SpinValueValidator<T>
{
  public:
    using BaseWidget = typename QtSpinBox::getType<T>::Type;
    using BaseWidget::BaseWidget;
    QValidator::State validate( QString& input, int& ) const override;
};

} // namespace Widgets
} // namespace Gui
} // namespace Ra

#include <Gui/Widgets/ConstrainedNumericSpinBox.inl>
