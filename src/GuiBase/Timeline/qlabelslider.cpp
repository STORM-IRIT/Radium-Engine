#include <GuiBase/Timeline/qlabelslider.h>

#include <QMouseEvent>

namespace Ra::GuiBase {

QLabelSlider::QLabelSlider( QWidget* parent ) : QLabel( parent ) {}

void QLabelSlider::mousePressEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::LeftButton )
    {
        m_clicked = true;
        emit slide( event->x() );
    }
}

void QLabelSlider::mouseReleaseEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::LeftButton )
    {
        setStyleSheet( "background-color: gray" );

        m_clicked = false;
        emit slideRelease();
    }
}

void QLabelSlider::mouseMoveEvent( QMouseEvent* event ) {
    if ( m_clicked ) { emit slide( event->x() ); }
}

} // namespace Ra::GuiBase
