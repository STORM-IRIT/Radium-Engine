#ifndef RADIUMENGINE_QLABELSLIDER_H_
#define RADIUMENGINE_QLABELSLIDER_H_

#include <QLabel>

namespace Ra::GuiBase {

class QLabelSlider : public QLabel
{
    Q_OBJECT
  public:
    explicit QLabelSlider( QWidget* parent = nullptr );

  protected:
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;

  signals:
    void slide( int deltaX );
    void slideRelease();

  public slots:

  private:
    int m_x;
    bool m_clicked = false;

    bool m_align[3];
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_QLABELSLIDER_H_
