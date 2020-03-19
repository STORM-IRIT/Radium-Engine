#ifndef RADIUMENGINE_QWIDGETRULER_H_
#define RADIUMENGINE_QWIDGETRULER_H_

#include <GuiBase/Timeline/Configurations.h>

#include <QLabel>
#include <QSpacerItem>
#include <QWidget>

namespace Ra::GuiBase {
class QDoubleSpinBoxSmart;
}

namespace Ra::GuiBase {

class QWidgetRuler : public QWidget
{
    Q_OBJECT
  public:
    explicit QWidgetRuler( QWidget* parent = nullptr );

    double* getStep();
    int* getNbInterval();
    double* getPixPerSec();
    int drawRuler( int newWidth );
    double* getZero();
    double* getMaxDuration();
    bool* getTimescaleLock();
    bool* getSelectorLock();
    void setSpinStart( QDoubleSpinBoxSmart* value );
    void setSpinEnd( QDoubleSpinBoxSmart* value );
    void setSpinCursor( QDoubleSpinBoxSmart* value );
    void setSpinDuration( QDoubleSpinBoxSmart* value );

    void setShiftDown( bool* value );

    void setCtrlDown( bool* value );

  signals:
    void rulerZoomed( QWheelEvent* event, double xr );

  public slots:
    void onDrawRuler( int width );

  private:
    static const int s_nbSteps      = 11;
    const double m_steps[s_nbSteps] = {0.05, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0};

    bool* m_ctrlDown;
    bool* m_shiftDown;

    double m_step;
    double m_pixPerSec;
    double m_zero;
    double m_maxDuration;

    QDoubleSpinBoxSmart* m_spinStart;
    QDoubleSpinBoxSmart* m_spinEnd;
    QDoubleSpinBoxSmart* m_spinCursor;
    QDoubleSpinBoxSmart* m_spinDuration;

    int m_nbInterval{0};
    bool m_timescaleLock{true};
    bool m_selectorLock{true};

    bool m_align[2];
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QWIDGETRULER_H_
