#ifndef RADIUMENGINE_QDOUBLESPINBOX_SMART_HPP_
#define RADIUMENGINE_QDOUBLESPINBOX_SMART_HPP_

#include <QDoubleSpinBox>
#include <QEvent>
#include <QKeyEvent>
#include <QObject>
#include <QWidget>

namespace Ra::GuiBase {

class QDoubleSpinBoxSmart : public QDoubleSpinBox
{
    Q_OBJECT
  public:
    explicit QDoubleSpinBoxSmart( QWidget* parent = nullptr );

  protected:
    void keyPressEvent( QKeyEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QDOUBLESPINBOX_SMART_HPP_
