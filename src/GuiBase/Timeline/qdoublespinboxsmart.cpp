#include <GuiBase/Timeline/qdoublespinboxsmart.h>

#include <QWheelEvent>

namespace Ra::GuiBase {

QDoubleSpinBoxSmart::QDoubleSpinBoxSmart( QWidget* parent ) : QDoubleSpinBox( parent ) {}

void QDoubleSpinBoxSmart::wheelEvent( QWheelEvent* event ) {
    QDoubleSpinBox::wheelEvent( event );

    emit editingFinished();

    event->accept();
}

void QDoubleSpinBoxSmart::keyPressEvent( QKeyEvent* event ) {
    QDoubleSpinBox::keyPressEvent( event );
    switch ( event->key() )
    {
    case Qt::Key_Up:
    case Qt::Key_Down:
        emit editingFinished();
        break;
    }

    event->ignore(); // Esc
}

} // namespace Ra::GuiBase
