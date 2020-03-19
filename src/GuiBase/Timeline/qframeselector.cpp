#include <GuiBase/Timeline/qframeselector.h>

#include <GuiBase/Timeline/Timeline.h>

#include <QPainter>
#include <QTimer>
#include <QWheelEvent>
#include <QtGlobal>

#include <GuiBase/Timeline/qwidgetruler.h>

namespace Ra::GuiBase {

QFrameSelector::QFrameSelector( QWidget* parent ) : QFrame( parent ) {
    m_widgetRuler = static_cast<QWidgetRuler*>( parent );
    m_nbInterval  = m_widgetRuler->getNbInterval();
    m_step        = m_widgetRuler->getStep();
    m_pixPerSec   = m_widgetRuler->getPixPerSec();
    m_zero        = m_widgetRuler->getZero();
    m_duration    = m_widgetRuler->getMaxDuration();

    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
}

QFrameSelector::~QFrameSelector() {
    delete m_timer;
}

double QFrameSelector::nearestStep( double time ) const {
    double deltaT = TIMELINE_AUTO_SUGGEST_CURSOR_RADIUS / *m_pixPerSec;

    double minDist = m_durationSpin->maximum();
    double dist;

    double newCursor = time;

    for ( double keyPose : m_keyPoses )
    {
        dist = qAbs( keyPose - time );
        if ( dist < deltaT && dist < minDist )
        {

            minDist   = dist;
            newCursor = keyPose;
        }
        if ( time < keyPose ) break;
    }

    for ( int i = 0; i < *m_nbInterval - 1; ++i )
    {
        double pos = i * *m_step;
        dist       = qAbs( pos - time );
        if ( dist < deltaT && dist < minDist )
        {
            minDist   = dist;
            newCursor = pos;
        }

        pos  = i * *m_step + 0.5 * *m_step;
        dist = qAbs( pos - time );
        if ( dist < deltaT && dist < minDist )
        {
            minDist   = dist;
            newCursor = pos;
        }

        if ( time < pos ) break;
    }

    return newCursor;
}

// ------------------------------- PROTECTED ----------------------------------
void QFrameSelector::paintEvent( QPaintEvent* ) {

    QPainter painter( this );
    int h = height();
    int w = width();

    if ( !m_sliding ) { redrawPlayZone(); }

    // DRAW FRAME SCALE
    painter.setPen( QPen( Qt::lightGray ) );
    double frameDuration = 1.0 / TIMELINE_FPS;
    int hUp              = h / 3;
    double nbFrame       = *m_duration / frameDuration;
    for ( int i = 0; i < nbFrame; i++ )
    {
        int x = static_cast<int>( i * frameDuration * *m_pixPerSec + *m_zero );
        painter.drawLine( x, 0, x, hUp );
    }

    // DRAW CURSOR
    painter.setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
    int xCursor = static_cast<int>( *m_zero + m_cursor * *m_pixPerSec );
    painter.drawLine( xCursor, 0, xCursor, h );

    // DRAW KEYPOSES
    painter.setPen( QPen( QColor( 255, 255, 0, 255 ), 3 ) );
    int hTemp = h / 3 + 2;
    for ( double keyPose : m_keyPoses )
    {
        int xKeyPose = static_cast<int>( *m_zero + keyPose * *m_pixPerSec );
        painter.drawLine( xKeyPose, hTemp, xKeyPose, h );
    }

    // DRAW TIME SCALE
    int hDown = 2 * h / 3;
    painter.setPen( Qt::black );
    for ( int i = 1; i < *m_nbInterval; i++ )
    {
        int x = static_cast<int>( i * *m_step * *m_pixPerSec );
        painter.drawLine( x, hDown, x, h );
    }
    int hDown2 = 3 * h / 4;
    painter.setPen( Qt::darkGray );
    for ( int i = 1; i < *m_nbInterval - 1; i++ )
    {
        int middle = static_cast<int>( ( i + 0.5 ) * *m_step * *m_pixPerSec );
        painter.drawLine( middle, hDown2, middle, h );
    }

    if ( m_updateKeyPoseFlash > 0 )
    {

        if ( m_updateKeyPoseFlash % 2 == 0 )
        {
            painter.setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
            int xKeyPose = static_cast<int>( *m_zero + m_keyPoseFlash * *m_pixPerSec );
            painter.drawLine( xKeyPose, hTemp, xKeyPose, h );
        }

        if ( --m_updateKeyPoseFlash == 0 ) m_timer->stop();
    }

    // DRAW MIDDLE RULER SEPARATOR
    int gap = 5;
    painter.setPen( Qt::white );
    painter.drawLine( 0, h / 2 + gap + 2, w, h / 2 + gap + 2 );
    painter.drawLine( 0, h / 2 + gap + 1, w, h / 2 + gap + 1 );

    painter.setPen( Qt::darkGray );
    painter.drawLine( 0, h / 2 - gap - 1, w, h / 2 - gap - 1 );
    painter.drawLine( 0, h / 2 + gap, w, h / 2 + gap );

    painter.setPen( Qt::black );
    painter.drawLine( 0, h / 2 - gap - 2, w, h / 2 - gap - 2 );
}

void QFrameSelector::mousePressEvent( QMouseEvent* event ) {
    // ---------------------- LEFT CLICK --------------------------------------
    if ( event->button() == Qt::LeftButton )
    {
        double newCursor = qMax( ( event->x() - *m_zero ) / *m_pixPerSec, 0.0 );

        // move cursor without render
        if ( *m_ctrlDown ) { onChangeCursor( newCursor, false ); }
        // delete keyPoses between cursor and newCursor
        else if ( *m_shiftDown )
        { deleteZone( m_cursor, newCursor ); }
        // move cursor and update renderer
        else
        {
            onChangeCursor( newCursor );
            m_mouseLeftClicked = true;
        }

        // ------------------ RIGHT CLICK -------------------------------------
    }
    else if ( event->button() == Qt::RightButton )
    {
        double newPose = qMax( ( event->x() - *m_zero ) / *m_pixPerSec, 0.0 );

        auto it = m_keyPoses.find( m_cursor );

        // if already on keyPose, move current keyPose
        // ------------------- CURSOR ON KEYPOSE -----------------------
        if ( it != m_keyPoses.end() )
        {

            double nearest = nearestStep( newPose );
            // -------------- SINGLE MOVE -------------------------------------
            if ( *m_shiftDown )
            {
                // if no keyPose under mouse, move keyPose to newPose
                if ( m_keyPoses.find( nearest ) == m_keyPoses.end() )
                {

                    if ( qAbs( m_cursor - nearest ) > 1e-5 )
                    {
                        m_cursor = nearest;

                        size_t id = static_cast<size_t>( std::distance( m_keyPoses.begin(), it ) );
                        m_keyPoses.erase( it );
                        m_keyPoses.insert( m_cursor );

                        updateCursorSpin(); // to find keyPose here, yellow spinBox
                        update();

                        emit keyPoseMoved( id, m_cursor ); // EXTERNAL SIGNAL
                    }
                }

                // ---------- MULTIPLE MOVE -----------------------------------
            }
            else
            {
                auto itLeft = it;
                --itLeft;
                double left = ( it == m_keyPoses.begin() ) ? ( 0.0 ) : ( *itLeft );

                if ( nearest > left )
                {
                    double dist = nearest - m_cursor;
                    size_t id   = static_cast<size_t>( std::distance( m_keyPoses.begin(), it ) );
                    moveKeyPoses( dist, id );
                }
            }

            // ---------------- CURSOR NOT ON KEYPOSE --------------------------
        }
        else
        {

            // if shiftdown, slide first right keypose to the left
            // --------------- MOVE RIGHT KEYPOSE TO THE LEFT -----------------
            if ( *m_shiftDown )
            {
                auto it       = m_keyPoses.begin();
                size_t iRight = 0;
                while ( it != m_keyPoses.end() && *it < newPose )
                {
                    ++it;
                    ++iRight;
                }

                // if keyPoses on the right, remove or insert time
                if ( it != m_keyPoses.end() )
                {

                    double right = *it;
                    double dist  = newPose - right;
                    moveKeyPoses( dist, iRight );
                }

                // if not shiftdown, slide first left keypose to the right
                // ---------------- MOVE LEFT KEYPOSE TO THE RIGHT -----------
            }
            else
            {
                auto it      = m_keyPoses.rbegin();
                size_t iLeft = m_keyPoses.size() - 1;
                while ( it != m_keyPoses.rend() && *it > newPose )
                {
                    ++it;
                    --iLeft;
                }

                if ( it != m_keyPoses.rend() )
                {
                    double left = *it;
                    double dist = newPose - left;

                    moveKeyPoses( dist, iLeft );
                }
            }
        }

        // no catch mouse event
    }
    else
    {
        event->ignore();
        return;
    }
    event->accept();
}

void QFrameSelector::mouseMoveEvent( QMouseEvent* event ) {
    if ( m_mouseLeftClicked )
    {
        double newCursor = qMax( ( event->x() - *m_zero ) / *m_pixPerSec, 0.0 );

        onChangeCursor( newCursor );
    }
    else
    { event->ignore(); }
}

void QFrameSelector::mouseReleaseEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::LeftButton )
    {
        m_mouseLeftClicked = false;
        event->accept();
    }
    else
    { event->ignore(); }
}

// -------------------------- EXTERNAL SLOTS ----------------------------------
// EXTERNAL SLOT
void QFrameSelector::onAddingKeyPose( double time, bool internal /* = true */ ) {
    // by default (time = -1.0), add keyPose on cursor
    if ( static_cast<int>( time ) == -1 ) time = m_cursor;

    int nbKeyPoses = static_cast<int>( m_keyPoses.size() );
    m_keyPoses.insert( time );

    // if keyPose not already here
    if ( static_cast<int>( m_keyPoses.size() ) != nbKeyPoses )
    {
        updateCursorSpin();
        update();

        if ( internal )
        {
            emit keyPoseAdded( time ); // EXTERNAL SIGNAL
        }

        m_nbKeyPosesSpin->setValue( static_cast<int>( m_keyPoses.size() ) );

        // keyPose already here, change actual keyPose
    }
    else
    {
        auto it   = m_keyPoses.find( time );
        size_t id = static_cast<size_t>( std::distance( m_keyPoses.begin(), it ) );

        if ( internal )
        {
            emit keyPoseChanged( id ); // EXTERNAL SIGNAL
        }

        m_updateKeyPoseFlash = 6;
        m_keyPoseFlash       = time;

        m_timer->start( 50 );

        update();
    }
}

// EXTERNAL SLOT
void QFrameSelector::onClearKeyPoses() {
    m_keyPoses.clear();
    m_nbKeyPosesSpin->setValue( 0 );

    updateCursorSpin();
    update();
}

// EXTERNAL SLOT
void QFrameSelector::onChangeStart( double time, bool internal /* = true */ ) {
    double newStart = qMax( qMin( time, m_end ), 0.0 );

    bool out    = qAbs( newStart - time ) > 1e-5;
    bool change = qAbs( newStart - m_start ) > 1e-5;

    if ( change )
    {
        m_start = newStart;
        updateStartSpin();
        redrawPlayZone();

        // emit signal if time of emitter is internal changed due of limits
        if ( internal || out )
        {
            emit startChanged( m_start ); // EXTERNAL SIGNAL
        }
    }
    else
    {

        if ( out ) { updateStartSpin(); }
    }
}

// EXTERNAL SLOT
void QFrameSelector::onChangeEnd( double time, bool internal /* = true */ ) {
    double newEnd = qMin( qMax( time, m_start ), *m_duration );

    bool out    = qAbs( newEnd - time ) > 1e-5;
    bool change = qAbs( newEnd - m_end ) > 1e-5;

    // emit signal only if new value of end
    if ( change )
    {
        m_end = newEnd;
        updateEndSpin();
        update();

        if ( internal || out )
        {
            emit endChanged( m_end ); // EXTERNAL SIGNAL
        }
    }
    else
    {

        if ( out ) { updateEndSpin(); }
    }
}

// EXTERNAL SLOT (warning on using EXTERNAL SIGNAL)
void QFrameSelector::onChangeCursor( double time, bool internal /* = true */ ) {
    double newCursor = qMax( 0.0, time );

    if ( internal ) { newCursor = nearestStep( newCursor ); }

    bool out    = qAbs( newCursor - time ) > 1e-5;
    bool change = qAbs( newCursor - m_cursor ) > 1e-5;

    if ( change )
    {
        m_cursor = newCursor;
        updateCursorSpin();
        update();

        if ( internal || out )
        {
            emit cursorChanged( m_cursor ); // EXTERNAL SIGNAL
        }
    }
    else
    {

        if ( out ) { updateCursorSpin(); }
    }
}

// EXTERNAL SLOT
void QFrameSelector::onChangeDuration( double time, bool internal /* = true */ ) {
    double newDuration = qMax( time, 0.0 );

    bool out    = qAbs( newDuration - time ) > 1e-5;
    bool change = qAbs( newDuration - *m_duration ) > 1e-5;

    if ( change )
    {
        *m_duration = newDuration;
        m_widgetRuler->drawRuler( m_widgetRuler->minimumWidth() );
        updateDurationSpin();

        // emit signal if time of emitter is internal changed due of limits
        if ( internal || out ) { emit durationChanged( *m_duration ); }
    }
    else
    {

        if ( out ) { updateDurationSpin(); }
    }

    if ( *m_duration < m_start ) onChangeStart( *m_duration );

    if ( *m_duration < m_end ) onChangeEnd( *m_duration );

    // auto update
}

// -------------------------- INTERNAL SLOTS ----------------------------------
void QFrameSelector::onSlideLeftSlider( int deltaX ) {

    if ( !m_sliding )
    {
        m_leftSlider->setStyleSheet( "background-color: #00ff00" );
        m_sliding = true;
    }

    double newStart = m_start + deltaX / *m_pixPerSec;

    onChangeStart( newStart ); // EXTERNAL SLOT

    m_leftSpacer->setMinimumWidth(
        static_cast<int>( *m_zero + m_start * *m_pixPerSec - m_leftSlider->width() ) );
    m_playZone->setMinimumWidth( static_cast<int>( ( m_end - m_start ) * *m_pixPerSec ) );
}

void QFrameSelector::onSlideRightSlider( int deltaX ) {
    if ( !m_sliding )
    {
        m_rightSlider->setStyleSheet( "background-color: red" );
        m_sliding = true;
    }
    double newEnd = m_end + deltaX / *m_pixPerSec;

    onChangeEnd( newEnd ); // EXTERNAL SLOT

    m_playZone->setMinimumWidth( static_cast<int>( ( m_end - m_start ) * *m_pixPerSec ) );
}

void QFrameSelector::onLeftSlideRelease() {
    m_sliding = false;
}

void QFrameSelector::onRightSlideRelease() {
    m_sliding = false;
}

void QFrameSelector::onDeleteKeyPose() {
    auto it = m_keyPoses.find( m_cursor );

    if ( it != m_keyPoses.end() )
    {

        size_t id = static_cast<size_t>( std::distance( m_keyPoses.begin(), it ) );
        m_keyPoses.erase( it );

        updateCursorSpin();
        update();

        m_nbKeyPosesSpin->setValue( static_cast<int>( m_keyPoses.size() ) );
        emit keyPoseDeleted( id ); // EXTERNAL SIGNAL

        onSetCursorToNextKeyPose();
    }
}

void QFrameSelector::onSetCursorToStart() {
    onChangeCursor( m_start );
}

void QFrameSelector::onSetCursorToEnd() {
    onChangeCursor( m_end );
}

void QFrameSelector::onSetCursorToPreviousKeyPose() {
    auto it = m_keyPoses.rbegin();
    while ( it != m_keyPoses.rend() && *it >= m_cursor )
        it++;

    if ( it != m_keyPoses.rend() ) { onChangeCursor( *it ); }
}

void QFrameSelector::onSetCursorToNextKeyPose() {
    auto it = m_keyPoses.begin();
    while ( it != m_keyPoses.end() && *it <= m_cursor )
        it++;

    if ( it != m_keyPoses.end() ) { onChangeCursor( *it ); }
}

void QFrameSelector::onChangeStartSpin() {
    onChangeStart( m_startSpin->value() );
}

void QFrameSelector::onChangeEndSpin() {
    onChangeEnd( m_endSpin->value() );
}

void QFrameSelector::onChangeCursorSpin() {
    onChangeCursor( m_cursorSpin->value() );
}

void QFrameSelector::onChangeDurationSpin() {
    onChangeDuration( m_durationSpin->value() );
}

// -------------------------- PRIVATE FUNCTIONS -------------------------------
void QFrameSelector::updateCursorSpin() {
    if ( m_keyPoses.find( m_cursor ) != m_keyPoses.end() )
    {
        m_cursorSpin->setStyleSheet( "background-color: yellow" );
        m_removeKeyPoseButton->setEnabled( true );
    }
    else
    {
        m_cursorSpin->setStyleSheet( "background-color: #5555ff" );
        m_removeKeyPoseButton->setEnabled( false );
    }
    m_cursorSpin->setValue( m_cursor );
}

void QFrameSelector::updateStartSpin() {
    m_startSpin->setValue( m_start );
}

void QFrameSelector::updateEndSpin() {
    m_endSpin->setValue( m_end );
}

void QFrameSelector::updateDurationSpin() {
    m_durationSpin->setValue( *m_duration );
}

void QFrameSelector::moveKeyPoses( double gap, size_t iFirst ) {
    std::set<double> clone;
    size_t i = 0;
    double first{0};
    for ( double d : m_keyPoses )
    {
        if ( i < iFirst )
            clone.insert( d );
        else
        {
            if ( i == iFirst ) first = d;

            clone.insert( d + gap );
        }
        i++;
    }
    m_keyPoses = clone;

    double left = ( gap > 0 ) ? ( first ) : ( first + gap );

    // emit keyPosesMoved before emitting cursorChanged to render the truly pose on cursor
    emit keyPosesMoved( gap, iFirst ); // EXTERNAL SIGNAL

    if ( m_start >= left )
    {
        //        start += gap;
        m_start = qMax( qMin( m_start + gap, *m_duration ), 0.0 );
        updateStartSpin();

        emit startChanged( m_start );
    }

    if ( m_end >= left )
    {
        //        end += gap;
        m_end = qMax( qMin( m_end + gap, *m_duration ), 0.0 );
        updateEndSpin();

        emit endChanged( m_end );
    }

    if ( m_cursor >= left )
    {
        bool cursorOnKeyPose = qAbs( m_cursor - first ) < 1e-5;

        m_cursor = qMax( m_cursor + gap, 0.0 );
        updateCursorSpin();

        if ( !cursorOnKeyPose ) { emit cursorChanged( m_cursor ); }
    }

    update();
}

void QFrameSelector::deleteZone( double time, double time2 ) {
    double left  = qMin( time, time2 );
    double right = qMax( time, time2 );

    double dist = right - left;

    auto it    = m_keyPoses.begin();
    size_t id  = 0;
    bool first = true;
    while ( it != m_keyPoses.end() )
    {
        double keyPose = *it;

        if ( keyPose >= left )
        {
            it = m_keyPoses.erase( it );

            if ( keyPose > right )
            {
                if ( first )
                {
                    emit keyPosesMoved( -dist, id );
                    first = false;
                }
                m_keyPoses.insert( keyPose - dist );
            }
            else
            { emit keyPoseDeleted( id ); }
        }
        else
        {
            ++it;
            ++id;
        }
    }
    m_nbKeyPosesSpin->setValue( static_cast<int>( m_keyPoses.size() ) );

    double newStart = qMax( qMax( qMin( m_start, left ), m_start - dist ), 0.0 );
    if ( qAbs( newStart - m_start ) > 1e-5 )
    {
        m_start = newStart;
        updateStartSpin();

        emit startChanged( m_start );
    }

    double newEnd = qMax( qMax( qMin( m_end, left ), m_end - dist ), 0.0 );
    if ( qAbs( newEnd - m_end ) > 1e-5 )
    {
        m_end = newEnd;
        updateEndSpin();

        emit endChanged( m_end );
    }

    update();
}

void QFrameSelector::redrawPlayZone() {
    m_leftSpacer->setMinimumWidth(
        static_cast<int>( *m_zero + m_start * *m_pixPerSec - m_leftSlider->width() ) );
    m_playZone->setMinimumWidth( static_cast<int>( ( m_end - m_start ) * *m_pixPerSec ) );
}

// -------------------------- GETTERS -----------------------------------------
double QFrameSelector::getStart() const {
    return m_start;
}

double* QFrameSelector::getStart() {
    return &m_start;
}

double* QFrameSelector::getEnd() {
    return &m_end;
}

double* QFrameSelector::getCursor() {
    return &m_cursor;
}

double QFrameSelector::getEnd() const {
    return m_end;
}

double QFrameSelector::getCursor() const {
    return m_cursor;
}

int QFrameSelector::getNbKeyPoses() const {
    return static_cast<int>( m_keyPoses.size() );
}

double QFrameSelector::getKeyPose( int id ) const {
    auto it = m_keyPoses.begin();
    while ( it != m_keyPoses.end() && id-- > 0 )
        it++;

    return *it;
}

std::set<double> QFrameSelector::getKeyPoses() const {
    return m_keyPoses;
}

std::set<double>* QFrameSelector::getKeyPoses() {
    return &m_keyPoses;
}

// -------------------------- SETTERS -----------------------------------------
void QFrameSelector::setCursor( double time ) {
    m_cursor = time;
}

void QFrameSelector::setKeyPoses( const std::set<double>& value ) {
    m_keyPoses = value;
}

void QFrameSelector::setShiftDown( bool* value ) {
    m_shiftDown = value;
}

void QFrameSelector::setStart( double value ) {
    m_start = value;
}

void QFrameSelector::setEnd( double value ) {
    m_end = value;
}

void QFrameSelector::setDuration( double time ) {
    *m_duration = time;
}

//
// -------------------------- REFERENCES SETTERS ------------------------------
void QFrameSelector::setLeftSpacer( QFrame* value ) {
    m_leftSpacer = value;
}

void QFrameSelector::setLeftSlider( QLabel* value ) {
    m_leftSlider = value;
}

void QFrameSelector::setPlayZone( QFrame* value ) {
    m_playZone = value;
}

void QFrameSelector::setRightSlider( QLabel* value ) {
    m_rightSlider = value;
}
//

void QFrameSelector::setCursorSpin( QDoubleSpinBox* value ) {
    m_cursorSpin = value;
}

void QFrameSelector::setStartSpin( QDoubleSpinBox* value ) {
    m_startSpin = value;
}

void QFrameSelector::setEndSpin( QDoubleSpinBox* value ) {
    m_endSpin = value;
}

void QFrameSelector::setDurationSpin( QDoubleSpinBox* value ) {
    m_durationSpin = value;
}

void QFrameSelector::setRemoveKeyPoseButton( QToolButton* value ) {
    m_removeKeyPoseButton = value;
}

void QFrameSelector::setStartInc( QDoubleSpinBox* value ) {
    m_startInc = value;
}

void QFrameSelector::setEndInc( QDoubleSpinBox* value ) {
    m_endInc = value;
}

void QFrameSelector::setNbKeyPosesSpin( QSpinBox* value ) {
    m_nbKeyPosesSpin = value;
}
void QFrameSelector::setCtrlDown( bool* value ) {
    m_ctrlDown = value;
}

void QFrameSelector::setMidMouseDown( bool* value ) {
    m_midMouseDown = value;
}

} // namespace Ra::GuiBase
