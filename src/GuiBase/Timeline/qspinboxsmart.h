#ifndef RADIUMENGINE_QSPINBOXSMART_H_
#define RADIUMENGINE_QSPINBOXSMART_H_

#include <QObject>
#include <QSpinBox>
#include <QWidget>

namespace Ra::GuiBase {

class QSpinBoxSmart : public QSpinBox
{
    Q_OBJECT
  public:
    explicit QSpinBoxSmart( QWidget* parent = nullptr );

  protected:
    void wheelEvent( QWheelEvent* event ) override;
    void keyPressEvent( QKeyEvent* event ) override;

  signals:
    void nextKeyPose();
    void previousKeyPose();
    void deleteKeyPose();
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QSPINBOXSMART_H_
