#ifndef RADIUMENGINE_QSCROLLAREARULER_H_
#define RADIUMENGINE_QSCROLLAREARULER_H_

#include <QObject>
#include <QScrollArea>
#include <QWidget>

namespace Ra::GuiBase {
class QWidgetRuler;
class QToolButtonPlayPause;
class QDoubleSpinBoxSmart;
class Timeline;
class QFrameSelector;
} // namespace Ra::GuiBase

namespace Ra::GuiBase {

class QScrollAreaRuler : public QScrollArea
{
    Q_OBJECT
  public:
    explicit QScrollAreaRuler( QWidget* parent = nullptr );

    bool* getShiftDown();
    void setRuler( QWidgetRuler* value );
    void setPlayPause( QToolButtonPlayPause* value );
    void setTimeline( Timeline* value );
    void setSpinDuration( QDoubleSpinBoxSmart* value );
    bool* getCtrlDown();
    void setSelector( QFrameSelector* value );
    bool* getMidMouseDown();
    void setCursorSpin( QDoubleSpinBoxSmart* value );

  protected:
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;

    void wheelEvent( QWheelEvent* event ) override;

    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;

  signals:
    void keyPoseOnMouseAdded();
    void removeKeyPose();
    void previousKeyPose();
    void nextKeyPose();
    void durationChanged( double time );
    void addKeyPose();

    void undo();
    void redo();

  public slots:
    void onKeyPress( QKeyEvent* event );
    void onKeyRelease( QKeyEvent* event );

  private:
    int m_mousePosX;
    int m_sliderPos;

    QWidgetRuler* m_ruler;
    QToolButtonPlayPause* m_playPause;
    Timeline* m_timeline;
    QDoubleSpinBoxSmart* m_spinDuration;

    QDoubleSpinBoxSmart* m_cursorSpin;

    QFrameSelector* m_selector;

    bool m_ctrlDown     = false;
    bool m_shiftDown    = false;
    bool m_midMouseDown = false;
    bool m_align[5];
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QSCROLLAREARULER_H_
