#ifndef RADIUMENGINE_QTOOLBUTTONPLAYPAUSE_H_
#define RADIUMENGINE_QTOOLBUTTONPLAYPAUSE_H_

#include <QObject>
#include <QToolButton>

namespace Ra::GuiBase {

class QToolButtonPlayPause : public QToolButton
{
    Q_OBJECT
  public:
    explicit QToolButtonPlayPause( QWidget* parent = nullptr );
    ~QToolButtonPlayPause() override;

    bool* getPlay();

  protected:
    void mousePressEvent( QMouseEvent* event ) override;

  signals:
    void playClicked();  // EXTERNAL SIGNAL
    void pauseClicked(); // EXTERNAL SIGNAL

  public slots:
    void onPlayMode();  // EXTERNAL SLOT
    void onPauseMode(); // EXTERNAL SLOT

    void onChangeMode();

  private:
    QIcon* m_playIcon;
    QIcon* m_pauseIcon;

    bool m_play = false;
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QTOOLBUTTONPLAYPAUSE_H_
