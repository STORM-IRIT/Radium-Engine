#include <GuiBase/KeyFrameEditor/KeyFrameEditorFrameScale.h>

#include <QPainter>
#include <QScrollBar>

#include <GuiBase/Timeline/Configurations.h>

#include <GuiBase/KeyFrameEditor/KeyFrameEditorFrame.h>
#include <GuiBase/KeyFrameEditor/KeyFrameEditorScrollArea.h>

namespace Ra::GuiBase {

KeyFrameEditorFrameScale::KeyFrameEditorFrameScale( QWidget* parent ) : QFrame( parent ) {}

void KeyFrameEditorFrameScale::setScrollArea( KeyFrameEditorScrollArea* scrollArea ) {
    m_scrollArea = scrollArea;
}

void KeyFrameEditorFrameScale::setEditorFrame( KeyFrameEditorFrame* editorFrame ) {
    m_editorFrame = editorFrame;
}

void KeyFrameEditorFrameScale::paintEvent( QPaintEvent* ) {
    if ( m_scrollArea == nullptr || m_editorFrame == nullptr ) { return; }

    QPainter painter( this );
    painter.setRenderHint( QPainter::HighQualityAntialiasing );

    int offset = m_scrollArea->horizontalScrollBar()->value();

    // DRAW FRAME SCALE
    painter.setPen( QPen( Qt::lightGray ) );
    Scalar frameDuration = 1_ra / TIMELINE_FPS;
    Scalar nbFrame       = m_scrollArea->getMaxTime() / frameDuration;
    Scalar pixPerSec     = m_scrollArea->getPixPerSec();
    Scalar zeroTime      = m_scrollArea->getZeroTime();
    for ( int i = 0; i < nbFrame; i++ )
    {
        int x = static_cast<int>( i * frameDuration * pixPerSec + zeroTime );
        painter.drawLine( x - offset, 0, x - offset, height() );
    }

    // DRAW KEYFRAMES
    painter.setPen( QPen( QColor( 255, 255, 0, 255 ), 3 ) );
    for ( Scalar keyFrame : m_editorFrame->getKeyFrames() )
    {
        int xKeyFrame = static_cast<int>( keyFrame * pixPerSec + zeroTime );
        painter.drawLine( xKeyFrame - offset, height() / 2, xKeyFrame - offset, height() );
    }
}

} // namespace Ra::GuiBase
