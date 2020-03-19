#include <GuiBase/KeyFrameEditor/KeyFrameEditorScrollArea.h>

#include <cmath>

#include <QScrollBar>
#include <QWheelEvent>

#include <GuiBase/Timeline/Timeline.h>
#include <GuiBase/Timeline/TimelineFrameSelector.h>

namespace Ra::GuiBase {

KeyFrameEditorScrollArea::KeyFrameEditorScrollArea( QWidget* parent ) : QScrollArea( parent ) {}

int KeyFrameEditorScrollArea::getNbIntervalTime() {
    return m_nbIntervalTime;
}

Scalar KeyFrameEditorScrollArea::getStepTime() {
    return m_stepTime;
}

Scalar KeyFrameEditorScrollArea::getPixPerSec() {
    return m_pixPerTime;
}

Scalar KeyFrameEditorScrollArea::getZeroTime() {
    return m_zeroTime;
}

Scalar KeyFrameEditorScrollArea::getMaxTime() {
    return m_maxTime;
}

void KeyFrameEditorScrollArea::setMaxTime( Scalar maxTime ) {
    m_maxTime = maxTime;
}

int KeyFrameEditorScrollArea::getNbIntervalValue() {
    return m_nbIntervalValue;
}

Scalar KeyFrameEditorScrollArea::getStepValue() {
    return m_stepValue;
}

Scalar KeyFrameEditorScrollArea::getPixPerValue() {
    return m_pixPerValue;
}

Scalar KeyFrameEditorScrollArea::getZeroValue() {
    return m_zeroValue;
}

Scalar KeyFrameEditorScrollArea::getMaxValue() {
    return m_maxValue;
}

void KeyFrameEditorScrollArea::onDrawRuler( int width, int height ) {
    int iStep = 0;
    while ( iStep < s_nbSteps && width * s_steps[iStep] < 50 * m_maxTime )
        iStep++;

    if ( iStep == s_nbSteps ) { return; }
    m_stepTime = s_steps[iStep];

    iStep = 0;
    while ( iStep < s_nbSteps && height * s_steps[iStep] < 50 * m_maxValue )
        iStep++;

    if ( iStep == s_nbSteps ) { return; }
    m_stepValue = s_steps[iStep];

    emit stepChanged( m_stepTime );

    m_nbIntervalTime = int( std::ceil( m_maxTime / m_stepTime ) ) + 2;
    m_pixPerTime     = ( Scalar( width ) / m_nbIntervalTime ) / m_stepTime;
    m_zeroTime       = m_pixPerTime * m_stepTime;

    m_nbIntervalValue = int( std::ceil( m_maxValue / m_stepValue ) ) + 2;
    m_pixPerValue     = ( Scalar( height ) / m_nbIntervalValue ) / m_stepValue;
    m_zeroValue       = height / 2; // m_pixPerValue * m_stepValue;

    widget()->setMinimumWidth( width );
    widget()->setMinimumHeight( height );

    update();
}

void KeyFrameEditorScrollArea::keyPressEvent( QKeyEvent* event ) {
    switch ( event->key() )
    {

    case Qt::Key_Space:
        emit togglePlayPause();
        break;

    case Qt::Key_Delete:
        emit removeKeyFrame();
        break;

    case Qt::Key_I:
        if ( event->modifiers() & Qt::Modifier::SHIFT ) { emit removeKeyFrame(); }
        else
        { emit addKeyFrame(); }
        break;

    case Qt::Key_Left:
        emit previousKeyFrame();
        break;

    case Qt::Key_Right:
        emit nextKeyFrame();
        break;

    case Qt::Key_Z:
        if ( event->modifiers() & Qt::Modifier::CTRL )
        {
            if ( event->modifiers() & Qt::Modifier::SHIFT ) { emit redo(); }
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

void KeyFrameEditorScrollArea::wheelEvent( QWheelEvent* event ) {
    const auto finishZoom = [this, event]( int w, int h ) {
        double hScroll = horizontalScrollBar()->value();
        double x       = event->x();
        double time    = ( hScroll + x - double( m_zeroTime ) ) / double( m_pixPerTime );

        double vScroll = verticalScrollBar()->value();
        double y       = event->y();
        double value   = ( vScroll + y - double( m_zeroValue ) ) / double( m_pixPerValue );

        onDrawRuler( w, h );

        // ruler values may have changed
        double a                      = time * double( m_pixPerTime ) + double( m_zeroTime );
        double hScrollAfterProjection = a - x;
        horizontalScrollBar()->setValue( int( hScrollAfterProjection ) );

        double b                      = value * double( m_pixPerValue ) + double( m_zeroValue );
        double vScrollAfterProjection = b - y;
        verticalScrollBar()->setValue( int( vScrollAfterProjection ) );
    };

    int ry         = event->angleDelta().ry();
    bool altDown   = event->modifiers() & Qt::Modifier::ALT;
    bool ctrlDown  = event->modifiers() & Qt::Modifier::CTRL;
    bool shiftDown = event->modifiers() & Qt::Modifier::SHIFT;
    // asymetric zoom
    if ( altDown )
    {
        ry                 = event->angleDelta().rx();
        int newRulerWidth  = widget()->minimumWidth();
        int newRulerHeight = widget()->minimumHeight();
        if ( shiftDown )
        {
            newRulerWidth = int( widget()->minimumWidth() +
                                 ry * TIMELINE_ZOOM_SPEED * widget()->minimumWidth() / width() );
            if ( newRulerWidth <= width() - 2 )
            {
                if ( widget()->minimumWidth() == width() - 2 ) { return; }
                else
                { newRulerWidth = width() - 2; }
            }
        }
        else
        {
            newRulerHeight = int( widget()->minimumHeight() +
                                  ry * TIMELINE_ZOOM_SPEED * widget()->minimumHeight() / height() );
            if ( newRulerHeight <= height() - 2 )
            {
                if ( widget()->minimumHeight() == height() - 2 ) { return; }
                else
                { newRulerHeight = height() - 2; }
            }
        }

        finishZoom( newRulerWidth, newRulerHeight );
    }
    // scroll up/down
    else if ( shiftDown )
    {
        verticalScrollBar()->setValue(
            static_cast<int>( verticalScrollBar()->value() + ry * TIMELINE_SLIDE_SPEED ) );
    }
    // scroll left/right
    else if ( ctrlDown )
    {
        horizontalScrollBar()->setValue(
            static_cast<int>( horizontalScrollBar()->value() + ry * TIMELINE_SLIDE_SPEED ) );
    }
    // symetric zoom in/out
    else
    {
        const int minWidth = widget()->minimumWidth();
        int newRulerWidth =
            static_cast<int>( minWidth + ry * TIMELINE_ZOOM_SPEED * minWidth / width() );
        if ( newRulerWidth <= width() - 2 )
        {
            if ( minWidth == width() - 2 ) { newRulerWidth = minWidth; }
            else
            { newRulerWidth = width() - 2; }
        }

        const int minHeight = widget()->minimumHeight();
        int newRulerHeight =
            static_cast<int>( minHeight + ry * TIMELINE_ZOOM_SPEED * minHeight / height() );
        if ( newRulerHeight <= height() - 2 )
        {
            if ( minHeight == height() - 2 ) { newRulerHeight = minHeight; }
            else
            { newRulerHeight = height() - 2; }
        }

        finishZoom( newRulerWidth, newRulerHeight );
    }
    event->accept(); // parent is animTimeline (root) with non event catching
    update();
}

void KeyFrameEditorScrollArea::mousePressEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::MiddleButton )
    {
        setCursor( Qt::SizeAllCursor );
        m_mousePosX  = event->x();
        m_mousePosY  = event->y();
        m_sliderPosX = horizontalScrollBar()->value();
        m_sliderPosY = verticalScrollBar()->value();
    }
}

void KeyFrameEditorScrollArea::mouseReleaseEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::MiddleButton ) { setCursor( Qt::ArrowCursor ); }
}

void KeyFrameEditorScrollArea::mouseMoveEvent( QMouseEvent* event ) {
    if ( event->buttons() & Qt::MiddleButton )
    {
        horizontalScrollBar()->setValue( ( m_sliderPosX + m_mousePosX - event->x() ) );
        verticalScrollBar()->setValue( ( m_sliderPosY + m_mousePosY - event->y() ) );
        update();
    }
}

} // namespace Ra::GuiBase
