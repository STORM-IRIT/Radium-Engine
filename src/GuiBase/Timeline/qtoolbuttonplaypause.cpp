#include <GuiBase/Timeline/qtoolbuttonplaypause.h>

#include <QMouseEvent>

namespace Ra::GuiBase {

QToolButtonPlayPause::QToolButtonPlayPause( QWidget* parent ) : QToolButton( parent ) {

    m_playIcon = new QIcon();
    m_playIcon->addPixmap( QPixmap( ":/images/play.png" ) );
    m_pauseIcon = new QIcon();
    m_pauseIcon->addPixmap( QPixmap( ":/images/pause.png" ) );

    this->setIcon( *m_playIcon );
}

QToolButtonPlayPause::~QToolButtonPlayPause() {
    delete m_playIcon;
    delete m_pauseIcon;
}

void QToolButtonPlayPause::mousePressEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::LeftButton )
    {
        onChangeMode();
        event->accept();
    }
}

// EXTERNAL SLOT
void QToolButtonPlayPause::onPlayMode() {
    if ( m_play ) { return; }

    this->setIcon( *m_pauseIcon );
    m_play = true;
}

// EXTERNAL SLOT
void QToolButtonPlayPause::onPauseMode() {
    if ( !m_play ) { return; }

    this->setIcon( *m_playIcon );
    m_play = false;
}

void QToolButtonPlayPause::onChangeMode() {
    if ( m_play )
    {
        onPauseMode();
        emit pauseClicked();
    }
    else
    {
        onPlayMode();
        emit playClicked();
    }
}

bool* QToolButtonPlayPause::getPlay() {
    return &m_play;
}

} // namespace Ra::GuiBase
