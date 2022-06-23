#pragma once
#include <Gui/RaGui.hpp>

#include <QDoubleSpinBox>
#include <QSpinBox>

#include <QWidget>

namespace Ra {
namespace Gui {
namespace Widgets {

template <typename T>
class SpinValueValidator
{
  public:
    using Predicate = std::function<bool( T )>;
    inline void setPredicate( Predicate p ) { m_p = p; }

    inline bool isValid( T s ) const { return m_p( s ); }

  private:
    Predicate m_p = []( T ) { return true; };
};

namespace QtSpinBox {
template <typename T>
struct getType {
    using Type = QSpinBox;
};

template <>
struct getType<float> {
    using Type = QDoubleSpinBox;
};

template <>
struct getType<double> {
    using Type = QDoubleSpinBox;
};
} // namespace QtSpinBox

template <typename T>
class RA_GUI_API CheckingSpinBox : public QtSpinBox::getType<T>::Type, public SpinValueValidator<T>
{
  public:
    using BaseWidget = typename QtSpinBox::getType<T>::Type;
    using BaseWidget::BaseWidget;
    QValidator::State validate( QString& input, int& ) const override;
};

} // namespace Widgets
} // namespace Gui
} // namespace Ra

#include <Gui/Widgets/CheckingSpinBox.inl>
