#include <GuiBase/Timeline/qscrollarearuler.h>

#include <QScrollBar>
#include <QWheelEvent>
#include <QtMath>

#include <GuiBase/Timeline/Timeline.h>
#include <GuiBase/Timeline/qdoublespinboxsmart.h>
#include <GuiBase/Timeline/qframeselector.h>
#include <GuiBase/Timeline/qtoolbuttonplaypause.h>
#include <GuiBase/Timeline/qwidgetruler.h>

namespace Ra::GuiBase {

QScrollAreaRuler::QScrollAreaRuler( QWidget* parent ) : QScrollArea( parent ) {

    horizontalScrollBar()->setStyleSheet( "\
                    QScrollBar:horizontal {\
                        background: transparent;\
                    }\
                    \
                    QScrollBar::handle:horizontal {\
                        background: white;\
                    }\
                    \
                    QScrollBar::add-line:horizontal {\
                        border: none;\
                        background: transparent;\
                    }\
                    \
                    QScrollBar::sub-line:horizontal {\
                        border: none;\
                        background: transparent;\
                    }" );
}

void QScrollAreaRuler::keyPressEvent( QKeyEvent* event ) {
    switch ( event->key() )
    {

    case Qt::Key_Control:
        m_ctrlDown = true;
        break;

    case Qt::Key_Space:
        m_playPause->onChangeMode();
        break;

    case Qt::Key_Delete:
        emit removeKeyPose();
        break;

    case Qt::Key_I:
        if ( m_shiftDown ) { emit removeKeyPose(); }
        else
        {
            emit addKeyPose();
            // TODO : add key pose on mouse if no key pose selected
        }
        break;

    case Qt::Key_Shift:
        m_shiftDown = true;
        break;

    case Qt::Key_Left:
        emit previousKeyPose();
        break;

    case Qt::Key_Right:
        emit nextKeyPose();
        break;

    case Qt::Key_Escape:
        m_timeline->hide();
        break;

    case Qt::Key_Up:
        emit durationChanged( m_spinDuration->value() + m_spinDuration->singleStep() );
        break;

    case Qt::Key_Down:
        emit durationChanged( m_spinDuration->value() - m_spinDuration->singleStep() );
        break;

    case Qt::Key_Z:
        if ( m_ctrlDown )
        {
            if ( m_shiftDown ) { emit redo(); }
            else
            { emit undo(); }
        }
        break;

    case Qt::Key_U:
        emit undo();
        break;

    case Qt::Key_R:
        emit redo();
    }
}

void QScrollAreaRuler::keyReleaseEvent( QKeyEvent* event ) {
    switch ( event->key() )
    {

    case Qt::Key_Control:
        m_ctrlDown = false;
        break;

    case Qt::Key_Shift:
        m_shiftDown = false;
        break;
    }
}

void QScrollAreaRuler::wheelEvent( QWheelEvent* event ) {
    int ry = event->angleDelta().ry();

    // change dialog width
    if ( m_shiftDown && m_ctrlDown )
    {
        int curWidth = m_timeline->width();
        int minWidth = m_timeline->minimumWidth();

        if ( curWidth == minWidth && ry <= 0 ) return;

        if ( ry + curWidth <= minWidth ) ry = minWidth - curWidth;

        int newX      = m_timeline->x() - ry;
        int newY      = m_timeline->y() + 1;
        int newWidth  = m_timeline->width() + 1 + ry;
        int newHeight = m_timeline->height();

        m_ruler->onDrawRuler( newWidth - 2 );
        m_timeline->setGeometry( newX, newY, newWidth, newHeight );
    }
    // next/previous keyPose
    else if ( m_shiftDown )
    {
        if ( ry > 0 ) { emit nextKeyPose(); }
        else
        { emit previousKeyPose(); }
    }
    // scroll left/right bar
    else if ( m_ctrlDown )
    {
        horizontalScrollBar()->setValue(
            static_cast<int>( horizontalScrollBar()->value() + ry * TIMELINE_SLIDE_SPEED ) );
    }
    // zoom in/out
    else
    {
        int newRulerWidth =
            static_cast<int>( m_ruler->minimumWidth() +
                              ry * TIMELINE_ZOOM_SPEED * m_ruler->minimumWidth() / width() );
        if ( newRulerWidth <= width() - 2 )
        {
            if ( m_ruler->minimumWidth() == width() - 2 ) { return; }
            else
            { newRulerWidth = width() - 2; }
        }

        double hScroll = horizontalScrollBar()->value();
        double x       = event->x();

        double* zero      = m_ruler->getZero();
        double* pixPerSec = m_ruler->getPixPerSec();

        double time = ( hScroll + x - *zero ) / *pixPerSec;
        time        = m_selector->nearestStep( time );

        m_ruler->onDrawRuler( newRulerWidth );

        double a = time * *pixPerSec + *zero;

        double hScrollAfterProjection = a - x;

        horizontalScrollBar()->setValue( static_cast<int>( hScrollAfterProjection ) );
    }
    event->accept(); // parent is animTimeline (root) with non event catching
}

void QScrollAreaRuler::mousePressEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::MiddleButton )
    {
        setCursor( Qt::SplitHCursor );
        m_mousePosX = event->x();
        m_sliderPos = horizontalScrollBar()->value();

        m_midMouseDown = true;
    }
}

void QScrollAreaRuler::mouseReleaseEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::MiddleButton )
    {
        m_midMouseDown = false;
        setCursor( Qt::ArrowCursor );
    }
}

void QScrollAreaRuler::mouseMoveEvent( QMouseEvent* event ) {
    if ( m_midMouseDown )
    { horizontalScrollBar()->setValue( ( m_sliderPos + m_mousePosX - event->x() ) ); }
}

void QScrollAreaRuler::setTimeline( Timeline* value ) {
    m_timeline = value;
}

void QScrollAreaRuler::setPlayPause( QToolButtonPlayPause* value ) {
    m_playPause = value;
}

void QScrollAreaRuler::setRuler( QWidgetRuler* value ) {
    m_ruler = value;
}

bool* QScrollAreaRuler::getShiftDown() {
    return &m_shiftDown;
}

void QScrollAreaRuler::onKeyPress( QKeyEvent* event ) {
    switch ( event->key() )
    {
    case Qt::Key_Right:
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Down:
        return;
    }
    keyPressEvent( event );
}

void QScrollAreaRuler::onKeyRelease( QKeyEvent* event ) {
    keyReleaseEvent( event );
}

void QScrollAreaRuler::setCursorSpin( QDoubleSpinBoxSmart* value ) {
    m_cursorSpin = value;
}

bool* QScrollAreaRuler::getMidMouseDown() {
    return &m_midMouseDown;
}

void QScrollAreaRuler::setSelector( QFrameSelector* value ) {
    m_selector = value;
}

bool* QScrollAreaRuler::getCtrlDown() {
    return &m_ctrlDown;
}

void QScrollAreaRuler::setSpinDuration( QDoubleSpinBoxSmart* value ) {
    m_spinDuration = value;
}

} // namespace Ra::GuiBase
