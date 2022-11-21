#pragma once
#include <Gui/RaGui.hpp>

#include <QWidget>

#include <memory>
#include <string>

#include <vector>

namespace Ra {
namespace Gui {
namespace Widgets {

class RA_GUI_API VectorEditorSignals : public QWidget
{
    Q_OBJECT
  public:
    explicit VectorEditorSignals( QWidget* parent = nullptr ) : QWidget( parent ) {}
  signals:
    void valueChanged( const std::vector<int>& );
    void valueChanged( const std::vector<unsigned int>& );
    void valueChanged( const std::vector<float>& );
    void valueChanged( const std::vector<double>& );
};
/**
 * A Widget to edit vectors
 */
template <typename T = Scalar>
class VectorEditor : public VectorEditorSignals
{
  public:
    using WidgetType = typename QtSpinBox::getType<T>::Type;
    using SignalType = typename QtSpinBox::getType<T>::SignalType;
    /** Constructors
     */
    /** @{ */
    explicit VectorEditor( const std::vector<T>& vector, QWidget* parent = nullptr );
    VectorEditor( const VectorEditor& )            = delete;
    VectorEditor& operator=( const VectorEditor& ) = delete;
    VectorEditor( VectorEditor&& )                 = delete;
    VectorEditor&& operator=( VectorEditor&& )     = delete;
    /**@}*/

    /**
     * @return the value of the vector
     */
    const std::vector<T>& vector() const;

  private:
    std::vector<T> m_vector;
};
} // namespace Widgets
} // namespace Gui
} // namespace Ra

#include <Gui/Widgets/VectorEditor.inl>
