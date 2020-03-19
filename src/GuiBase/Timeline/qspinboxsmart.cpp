#include <GuiBase/Timeline/qspinboxsmart.h>

#include <QWheelEvent>

namespace Ra::GuiBase {

QSpinBoxSmart::QSpinBoxSmart( QWidget* parent ) : QSpinBox( parent ) {}

void QSpinBoxSmart::wheelEvent( QWheelEvent* event ) {

    int ry = ( event->angleDelta().ry() > 0 ) ? ( 1 ) : ( -1 );
    if ( ry > 0 ) { emit nextKeyPose(); }
    else
    { emit previousKeyPose(); }
    event->accept();
}

void QSpinBoxSmart::keyPressEvent( QKeyEvent* event ) {
    switch ( event->key() )
    {
    case Qt::Key_Delete:
        emit deleteKeyPose();
        break;

    default:
        event->ignore();
        return;
    }

    event->accept();
}

} // namespace Ra::GuiBase
