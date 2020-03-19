#ifndef RADIUMENGINE_QFRAMEBUTTONS_H_
#define RADIUMENGINE_QFRAMEBUTTONS_H_

#include <QFrame>
#include <QPoint>
#include <QScrollArea>
#include <QToolButton>

namespace Ra::GuiBase {
class Timeline;
class QWidgetRuler;
} // namespace Ra::GuiBase

namespace Ra::GuiBase {

class QFrameButtons : public QFrame
{
    Q_OBJECT
  public:
    explicit QFrameButtons( QWidget* parent = nullptr );

    void setTimeline( Timeline* value );
    void setRuler( QWidgetRuler* value );
    void setScrollArea( QScrollArea* value );
    void setHelpButton( QToolButton* value );

  protected:
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;

  signals:
    void keyPressed( QKeyEvent* event );
    void keyReleased( QKeyEvent* event );

  public slots:
    void helpClicked();

  private:
    Timeline* m_timeline;
    QWidgetRuler* m_ruler;
    QPoint m_offset;
    QScrollArea* m_scrollArea;

    QToolButton* m_helpButton;
    bool m_clicked = false;

    bool m_align[7];
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QFRAMEBUTTONS_H_
