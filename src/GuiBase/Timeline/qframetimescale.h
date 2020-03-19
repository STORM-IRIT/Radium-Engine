#ifndef RADIUMENGINE_QFRAMETIMESCALE_H_
#define RADIUMENGINE_QFRAMETIMESCALE_H_

#include <QFrame>
#include <QObject>

namespace Ra::GuiBase {
class QWidgetRuler;
}

namespace Ra::GuiBase {

class QFrameTimescale : public QFrame
{
    Q_OBJECT
  public:
    explicit QFrameTimescale( QWidget* parent = nullptr );

    void setDrawLock( bool* value );

  signals:

  protected:
    virtual void paintEvent( QPaintEvent* event ) override;

  public slots:

  private:
    QWidgetRuler* m_widgetRuler;

    int* m_nbInterval;
    double* m_step;
    double* m_pixPerSec;

    bool* m_drawLock;
    int m_counter{0};

    int m_align;
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QFRAMETIMESCALE_H_
