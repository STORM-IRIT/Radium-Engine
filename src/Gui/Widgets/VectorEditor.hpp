#pragma once
#include <Gui/RaGui.hpp>

#include <QWidget>

#include <memory>
#include <string>

#include <vector>

namespace Ra {
namespace Gui {
namespace Widgets {
/**
 * A Widget to edit vectors
 */
class RA_GUI_API VectorEditor : public QWidget
{
    Q_OBJECT
  public:
    /** Constructors
     */
    /** @{ */
    explicit VectorEditor( const std::vector<double>& vector,
                           int dec         = 3,
                           QWidget* parent = nullptr );
    VectorEditor( const VectorEditor& ) = delete;
    VectorEditor& operator=( const VectorEditor& ) = delete;
    VectorEditor( VectorEditor&& )                 = delete;
    VectorEditor&& operator=( VectorEditor&& ) = delete;
    /**@}*/

    /**
     * @return the value of the vector
     */
    const std::vector<double>& vector();

  signals:
    void valueChanged( const std::vector<double>& );

  private:
    std::vector<double> m_vector;
};
} // namespace Widgets
} // namespace Gui
} // namespace Ra
