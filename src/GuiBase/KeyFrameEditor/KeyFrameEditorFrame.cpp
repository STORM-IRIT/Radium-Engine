#include <GuiBase/KeyFrameEditor/KeyFrameEditorFrame.hpp>

#include <QCheckBox>
#include <QPainter>
#include <QScrollBar>
#include <QTimer>
#include <QWheelEvent>
#include <QtGlobal>
#include <QPainterPath>

#include <iostream>

#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Animation/KeyFramedValueInterpolators.hpp>
#include <Core/Math/Interpolation.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Log.hpp>

#include <GuiBase/KeyFrameEditor/KeyFrameEditorScrollArea.hpp>
#include <GuiBase/Timeline/Configurations.hpp>
#include <ui_KeyFrameEditor.h>

static constexpr int CTRL_PT_RAD = 5;

using namespace Ra::Core::Animation;
using namespace Ra::Core::Utils;

namespace Ra::GuiBase {

KeyFrameEditorFrame::KeyFrameEditorFrame( QWidget* parent ) : QFrame( parent ) {
    m_painter = new QPainter( this );
}

KeyFrameEditorFrame::~KeyFrameEditorFrame() {
    delete m_painter;
}

void KeyFrameEditorFrame::setEditorUi( Ui::KeyFrameEditor* ui ) {
    m_ui = ui;
    if ( m_ui == nullptr ) { return; }

    m_ui->m_currentTime_dsb->setMaximum( double( m_ui->scrollArea->getMaxTime() ) );

    // create connections
    connect( m_ui->m_currentTime_dsb,
             static_cast<void ( QDoubleSpinBox::* )( double )>( &QDoubleSpinBox::valueChanged ),
             [=]( double current ) { onChangeCursor( Scalar( current ) ); } );
    connect( ui->m_addKeyframe, &QToolButton::clicked, this, &KeyFrameEditorFrame::onAddKeyFrame );
    connect(
        ui->m_deleteKeyframe, &QToolButton::clicked, this, &KeyFrameEditorFrame::onDeleteKeyFrame );

    connect( ui->scrollArea,
             &KeyFrameEditorScrollArea::addKeyFrame,
             this,
             &KeyFrameEditorFrame::onAddKeyFrame );
    connect( ui->scrollArea,
             &KeyFrameEditorScrollArea::removeKeyFrame,
             this,
             &KeyFrameEditorFrame::onDeleteKeyFrame );
    connect( ui->scrollArea,
             &KeyFrameEditorScrollArea::nextKeyFrame,
             this,
             &KeyFrameEditorFrame::onSetCursorToNextKeyFrame );
    connect( ui->scrollArea,
             &KeyFrameEditorScrollArea::previousKeyFrame,
             this,
             &KeyFrameEditorFrame::onSetCursorToPreviousKeyFrame );

    connect( ui->scrollArea, &KeyFrameEditorScrollArea::stepChanged, [this]( Scalar step ) {
        m_ui->m_currentTime_dsb->setSingleStep( 0.5 * double( step ) );
    } );

    m_displayCurve[0] = m_ui->m_posX->isChecked();
    connect( m_ui->m_posX, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[0] = on;
        update();
    } );
    m_displayCurve[1] = m_ui->m_posY->isChecked();
    connect( m_ui->m_posY, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[1] = on;
        update();
    } );
    m_displayCurve[2] = m_ui->m_posZ->isChecked();
    connect( m_ui->m_posZ, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[2] = on;
        update();
    } );
    m_displayCurve[3] = m_ui->m_posW->isChecked();
    connect( m_ui->m_posW, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[3] = on;
        update();
    } );
    m_displayCurve[4] = m_ui->m_rotX->isChecked();
    connect( m_ui->m_rotX, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[4] = on;
        update();
    } );
    m_displayCurve[5] = m_ui->m_rotY->isChecked();
    connect( m_ui->m_rotY, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[5] = on;
        update();
    } );
    m_displayCurve[6] = m_ui->m_rotZ->isChecked();
    connect( m_ui->m_rotZ, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[6] = on;
        update();
    } );
    m_displayCurve[7] = m_ui->m_scaleX->isChecked();
    connect( m_ui->m_scaleX, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[7] = on;
        update();
    } );
    m_displayCurve[8] = m_ui->m_scaleY->isChecked();
    connect( m_ui->m_scaleY, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[8] = on;
        update();
    } );
    m_displayCurve[9] = m_ui->m_scaleZ->isChecked();
    connect( m_ui->m_scaleZ, &QCheckBox::toggled, [=]( bool on ) {
        m_displayCurve[9] = on;
        update();
    } );
}

void KeyFrameEditorFrame::setDuration( Scalar time ) {
    Scalar newDuration = std::max( time, 0_ra );

    bool change = qAbs( newDuration - m_ui->scrollArea->getMaxTime() ) > 1e-5_ra;

    if ( change )
    {
        m_ui->scrollArea->setMaxTime( newDuration );
        m_ui->scrollArea->onDrawRuler( m_ui->scrollArea->widget()->minimumWidth(),
                                       m_ui->scrollArea->widget()->minimumHeight() );
    }
    m_ui->m_currentTime_dsb->setMaximum( double( time ) );
}

void KeyFrameEditorFrame::setKeyFramedValue( KeyFrame* frame ) {
    m_value = frame;
    registerKeyFrames( true );
    update();
}

std::vector<Scalar> KeyFrameEditorFrame::getKeyFrames() const {
    return m_value->getTimes();
}

void KeyFrameEditorFrame::onChangeCursor( Scalar time, bool internal ) {
    Scalar newCursor = std::max( 0_ra, time );

    if ( internal ) { newCursor = nearestStep( newCursor ); }

    bool change = std::abs( newCursor - m_cursor ) > 1e-5_ra;

    if ( change )
    {
        m_cursor = newCursor;
        if ( internal ) { emit cursorChanged( m_cursor ); }
        update();
    }
    updateCursorSpin();
}

void KeyFrameEditorFrame::onUpdateKeyFrames( Scalar currentTime ) {
    m_cursor = currentTime;

    updateCursorSpin();
    m_ui->m_nbKeyFrame->setText( QString::number( m_value->getTimes().size() ) );

    registerKeyFrames();
    update();
}

void KeyFrameEditorFrame::onAddKeyFrame() {
    auto times = m_value->getTimes();
    auto it    = std::find_if( times.begin(), times.end(), [this]( const auto& t ) {
        return Ra::Core::Math::areApproxEqual( t, m_cursor );
    } );
    if ( it != times.end() ) { return; }

    emit keyFrameAdded( m_cursor );

    onUpdateKeyFrames( m_cursor );
}

void KeyFrameEditorFrame::onDeleteKeyFrame() {
    auto times = m_value->getTimes();
    auto it    = std::find_if( times.begin(), times.end(), [this]( const auto& t ) {
        return Ra::Core::Math::areApproxEqual( t, m_cursor );
    } );
    if ( it == times.end() ) { return; }

    emit keyFrameDeleted( std::distance( times.begin(), it ) );

    onUpdateKeyFrames( m_cursor );
}

void KeyFrameEditorFrame::onMoveKeyFrame( Scalar time0, Scalar time1 ) {
    if ( Ra::Core::Math::areApproxEqual( time0, time1 ) ) { return; }
    m_cursor = time1;

    auto times = m_value->getTimes();
    auto it    = std::find_if( times.begin(), times.end(), [time0]( const auto& t ) {
        return Ra::Core::Math::areApproxEqual( t, time0 );
    } );

    emit keyFrameMoved( std::distance( times.begin(), it ), time1 );

    onUpdateKeyFrames( m_cursor );
}

void KeyFrameEditorFrame::onMoveKeyFrames( Scalar time, Scalar offset ) {
    if ( Ra::Core::Math::areApproxEqual( offset, 0_ra ) ) { return; }

    auto times = m_value->getTimes();
    auto it    = std::find_if( times.begin(), times.end(), [time]( const auto& t ) {
        return Ra::Core::Math::areApproxEqual( t, time );
    } );
    if ( it == times.end() ) { return; }

    Scalar left = ( offset > 0 ) ? ( time ) : ( time + offset );

    emit keyFramesMoved( std::distance( times.begin(), it ), offset );

    if ( m_cursor >= left )
    {
        m_cursor = std::max( m_cursor + offset, 0_ra );
        updateCursorSpin();
        emit cursorChanged( m_cursor );
    }
    onUpdateKeyFrames( m_cursor );
}

void KeyFrameEditorFrame::deleteZone( Scalar time, Scalar time2 ) {
    Scalar left  = qMin( time, time2 );
    Scalar right = qMax( time, time2 );

    Scalar dist = right - left;

    auto times = m_value->getTimes();
    auto it    = times.begin();
    bool first = true;
    while ( it != times.end() )
    {
        Scalar keyFrame = *it;

        if ( keyFrame >= left )
        {
            if ( keyFrame > right )
            {
                if ( first )
                {
                    emit keyFramesMoved( std::distance( times.begin(), it ), -dist );
                    first = false;
                }
            }
            else
            { emit keyFrameDeleted( keyFrame ); }
        }
        ++it;
    }

    emit cursorChanged( m_cursor );

    onUpdateKeyFrames( m_cursor );
}

void KeyFrameEditorFrame::onSetCursorToPreviousKeyFrame() {
    auto times = m_value->getTimes();
    auto it    = times.rbegin();
    while ( it != times.rend() && *it >= m_cursor )
        it++;

    if ( it != times.rend() ) { onChangeCursor( *it ); }
}

void KeyFrameEditorFrame::onSetCursorToNextKeyFrame() {
    auto times = m_value->getTimes();
    auto it    = times.begin();
    while ( it != times.end() && *it <= m_cursor )
        it++;

    if ( it != times.end() ) { onChangeCursor( *it ); }
}

#define drawVector( VECTOR_TYPE, N )                                 \
    [&path, fromValueToPixel, this]( int x, const VECTOR_TYPE& v ) { \
        int y;                                                       \
        for ( uint i = 0; i < N; ++i )                               \
        {                                                            \
            if ( m_displayCurve[i] )                                 \
            {                                                        \
                fromValueToPixel( v( i ), y );                       \
                path[i].lineTo( QPoint( x, y ) );                    \
            }                                                        \
        }                                                            \
    }

#define drawSampled( drawFunc, type )                                \
    {                                                                \
        int p;                                                       \
        Scalar t0 = *times.begin();                                  \
        for ( auto t1 : times )                                      \
        {                                                            \
            fromTimeToPixel( t1, p );                                \
            if ( p < sliderH )                                       \
            {                                                        \
                std::exchange( t0, t1 );                             \
                continue;                                            \
            }                                                        \
            fromTimeToPixel( t0, p );                                \
            if ( p > sliderH + areaWidth )                           \
            {                                                        \
                std::exchange( t0, t1 );                             \
                continue;                                            \
            }                                                        \
            const int N = int( ( t1 - t0 ) / stepT );                \
            for ( Scalar t = t0; t <= t1; t += 1_ra / N )            \
            {                                                        \
                fromTimeToPixel( t, p );                             \
                drawFunc( p, kf->at( t, linearInterpolate<type> ) ); \
            }                                                        \
            std::exchange( t0, t1 );                                 \
        }                                                            \
    }

void KeyFrameEditorFrame::paintEvent( QPaintEvent* ) {
    int h            = height();
    int w            = width();
    int nT           = m_ui->scrollArea->getNbIntervalTime();
    Scalar stepT     = m_ui->scrollArea->getStepTime();
    Scalar zeroTime  = m_ui->scrollArea->getZeroTime();
    Scalar pixPerSec = m_ui->scrollArea->getPixPerSec();
    int nV           = m_ui->scrollArea->getNbIntervalValue();
    Scalar stepV     = m_ui->scrollArea->getStepValue();
    Scalar zeroValue = m_ui->scrollArea->getZeroValue();
    Scalar pixPerVal = m_ui->scrollArea->getPixPerValue();

    m_painter->begin( this );

    // DRAW GRID
    m_painter->setPen( Qt::darkGray );
    for ( int i = 1; i < nT; i++ )
    {
        int x = static_cast<int>( i * stepT * pixPerSec );
        m_painter->drawLine( x, 0, x, h );
    }
    m_painter->setPen( Qt::gray );
    for ( int i = 1; i < nT; i++ )
    {
        int middleX = int( ( i + 0.5_ra ) * stepT * pixPerSec );
        m_painter->drawLine( middleX, 0, middleX, h );
    }
    m_painter->setPen( Qt::darkGray );
    for ( int i = 1; i < nV; i++ )
    {
        int y = int( i * stepV * pixPerVal );
        m_painter->drawLine( int( zeroTime ), y, w, y );
    }
    m_painter->setPen( Qt::gray );
    for ( int i = 1; i < nV; i++ )
    {
        int middleY = int( ( i + 0.5_ra ) * stepV * pixPerVal );
        m_painter->drawLine( int( zeroTime ), middleY, w, middleY );
    }
    m_painter->setPen( QPen( Qt::black, 3 ) );
    m_painter->drawLine( int( zeroTime ), 0, int( zeroTime ), h );
    m_painter->drawLine(
        0, int( nV / 2 * stepV * pixPerVal ), w, int( nV / 2 * stepV * pixPerVal ) );

    // DRAW CURSOR
    m_painter->setPen( QPen( Qt::blue, 3 ) );
    int xCursor = static_cast<int>( zeroTime + m_cursor * pixPerSec );
    m_painter->drawLine( xCursor, 0, xCursor, h );

    // DRAW CURVES
    if ( m_curveControlPoints.size() )
    {
        m_painter->setPen( Qt::black );
        m_painter->setBrush( QColor( 255, 127, 0, 255 ) );
        static QColor curveColor[10] = {Qt::red,
                                        Qt::green,
                                        Qt::blue,
                                        Qt::darkYellow,
                                        Qt::red,
                                        Qt::green,
                                        Qt::blue,
                                        Qt::red,
                                        Qt::green,
                                        Qt::blue};
        const auto fromValueToPixel  = [=]( Scalar value, int& y ) {
            y = int( zeroValue - value * pixPerVal );
        };

        const auto fromTimeToPixel = [=]( Scalar time, int& x ) {
            x = int( zeroTime + time * pixPerSec );
        };

        const auto fromTimeValueToPixels = [=]( Scalar time, Scalar value, int& x, int& y ) {
            fromTimeToPixel( time, x );
            fromValueToPixel( value, y );
        };

        // fill strokes
        QPainterPath path[10] = {QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath(),
                                 QPainterPath()};

        // CURVES
        const auto drawFlat =
            [fromTimeValueToPixels]( const CurveControlPoints& curve, int y0, QPainterPath& path ) {
                int x, y;
                // flat between control points
                for ( const auto& P : curve )
                {
                    fromTimeValueToPixels( P.x(), P.y(), x, y );
                    path.lineTo( QPoint( x, y0 ) );
                    path.lineTo( QPoint( x, y ) );
                    y0 = y;
                }
            };

        const auto drawQuaternion =
            [&path, fromValueToPixel, this]( int x, const Ra::Core::Quaternion& Q ) {
                int y;
                auto angles = Q.matrix().eulerAngles( 0, 1, 2 );
                if ( m_displayCurve[4] )
                {
                    fromValueToPixel( angles.x(), y );
                    path[4].lineTo( QPoint( x, y ) );
                }
                if ( m_displayCurve[5] )
                {
                    fromValueToPixel( angles.y(), y );
                    path[5].lineTo( QPoint( x, y ) );
                }
                if ( m_displayCurve[6] )
                {
                    fromValueToPixel( angles.z(), y );
                    path[6].lineTo( QPoint( x, y ) );
                }
            };

        // first: flat before first sample
        int px, py;
        for ( size_t channel = 0; channel < 10; ++channel )
        {
            if ( !m_displayCurve[channel] ) continue;
            const auto& curve = m_curveControlPoints[channel];
            const auto& P     = curve[0];
            fromTimeValueToPixels( P.x(), P.y(), px, py );
            path[channel].moveTo( QPoint( int( zeroTime ), py ) );
            path[channel].lineTo( QPoint( px, py ) );
        }

        // then: sample between times
        const int sliderH   = m_ui->scrollArea->horizontalScrollBar()->value();
        const int areaWidth = m_ui->scrollArea->width();

        const auto times = m_value->getTimes();
        if ( auto kf = dynamic_cast<KeyFramedValue<bool>*>( m_value ) )
        {
            if ( m_displayCurve[0] ) { drawFlat( m_curveControlPoints[0], py, path[0] ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<int>*>( m_value ) )
        {
            if ( m_displayCurve[0] ) { drawFlat( m_curveControlPoints[0], py, path[0] ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Scalar>*>( m_value ) )
        {
            if ( m_displayCurve[0] )
            {
                const auto drawScalar = [&path, fromValueToPixel]( int x, Scalar v ) {
                    int y;
                    fromValueToPixel( v, y );
                    path[0].lineTo( QPoint( x, y ) );
                };
                drawSampled( drawScalar, Scalar );
            }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector2>*>( m_value ) )
        {
            if ( m_displayCurve[0] || m_displayCurve[1] )
            { drawSampled( drawVector( Ra::Core::Vector2, 2 ), Ra::Core::Vector2 ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector3>*>( m_value ) )
        {
            if ( m_displayCurve[0] || m_displayCurve[1] || m_displayCurve[2] )
            { drawSampled( drawVector( Ra::Core::Vector3, 3 ), Ra::Core::Vector3 ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector4>*>( m_value ) )
        {
            if ( m_displayCurve[0] || m_displayCurve[1] || m_displayCurve[2] || m_displayCurve[3] )
            { drawSampled( drawVector( Ra::Core::Vector4, 4 ), Ra::Core::Vector4 ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Utils::Color>*>( m_value ) )
        {
            if ( m_displayCurve[0] || m_displayCurve[1] || m_displayCurve[2] || m_displayCurve[3] )
            { drawSampled( drawVector( Ra::Core::Utils::Color, 4 ), Ra::Core::Utils::Color ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Quaternion>*>( m_value ) )
        {
            if ( m_displayCurve[0] || m_displayCurve[1] || m_displayCurve[2] || m_displayCurve[3] )
            { drawSampled( drawQuaternion, Ra::Core::Quaternion ); }
        }
        else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Transform>*>( m_value ) )
        {
            if ( m_displayCurve[0] || m_displayCurve[1] || m_displayCurve[2] || m_displayCurve[3] ||
                 m_displayCurve[4] || m_displayCurve[5] || m_displayCurve[6] || m_displayCurve[7] ||
                 m_displayCurve[8] || m_displayCurve[9] )
            {
                const auto drawTransform = [&path, fromValueToPixel, drawQuaternion, this](
                                               int x, const Ra::Core::Transform& T ) {
                    int y;
                    Ra::Core::Matrix3 R, S;
                    T.computeRotationScaling( &R, &S );
                    auto drawPos = drawVector( Ra::Core::Vector3, 3 );
                    drawPos( x, T.translation() );
                    drawQuaternion( x, Ra::Core::Quaternion( R ) );
                    if ( m_displayCurve[7] )
                    {
                        fromValueToPixel( S( 0, 0 ), y );
                        path[7].lineTo( QPoint( x, y ) );
                    }
                    if ( m_displayCurve[8] )
                    {
                        fromValueToPixel( S( 1, 1 ), y );
                        path[8].lineTo( QPoint( x, y ) );
                    }
                    if ( m_displayCurve[9] )
                    {
                        fromValueToPixel( S( 2, 2 ), y );
                        path[9].lineTo( QPoint( x, y ) );
                    }
                };
                drawSampled( drawTransform, Ra::Core::Transform );
            }
        }

        // finally: flat after last
        for ( size_t channel = 0; channel < 10; ++channel )
        {
            if ( !m_displayCurve[channel] ) continue;
            const auto& curve = m_curveControlPoints[channel];
            const auto& P     = curve.back();
            fromTimeValueToPixels( P.x(), P.y(), px, py );
            path[channel].lineTo( QPoint( px, py ) );
            path[channel].lineTo( QPoint( w, py ) );
        }
        // paint curves
        for ( size_t channel = 0; channel < 10; ++channel )
        {
            if ( !m_displayCurve[channel] ) continue;
            m_painter->strokePath( path[channel], QPen( curveColor[channel], 2 ) );
        }

        // CONTROL POINTS
        for ( size_t channel = 0; channel < 10; ++channel )
        {
            if ( !m_displayCurve[channel] ) { continue; }
            auto curve = m_curveControlPoints[channel];
            for ( const auto& P : curve )
            {
                fromTimeValueToPixels( P.x(), P.y(), px, py );
                m_painter->drawEllipse( QPoint( px, py ), CTRL_PT_RAD, CTRL_PT_RAD );
            }
        }

        // REDRAW CURRENT CONTROL POINT
        m_painter->setBrush( QColor( 255, 255, 0, 255 ) );
        int i = m_currentControlPoint( 0 );
        int j = m_currentControlPoint( 1 );
        if ( i != -1 && j != -1 )
        {
            const auto& P = m_curveControlPoints[uint( i )][uint( j )];
            fromTimeValueToPixels( P.x(), P.y(), px, py );
            m_painter->drawEllipse( QPoint( px, py ), CTRL_PT_RAD, CTRL_PT_RAD );
        }
    }

    m_painter->end();

    emit updated();
}
#undef drawSampled
#undef drawVector

void KeyFrameEditorFrame::mousePressEvent( QMouseEvent* event ) {
    Scalar zeroTime  = m_ui->scrollArea->getZeroTime();
    Scalar pixPerSec = m_ui->scrollArea->getPixPerSec();

    Scalar newFrame = std::max( ( event->x() - zeroTime ) / pixPerSec, 0.0_ra );
    bool shiftDown  = event->modifiers() & Qt::Modifier::SHIFT;
    bool ctrlDown   = event->modifiers() & Qt::Modifier::CTRL;
    // ---------------------- LEFT CLICK --------------------------------------
    if ( event->button() == Qt::LeftButton )
    {
        // check if grabbing a control point
        Scalar zeroValue = m_ui->scrollArea->getZeroValue();
        Scalar pixPerVal = m_ui->scrollArea->getPixPerValue();
        for ( size_t i = 0; i < m_curveControlPoints.size(); ++i )
        {
            if ( !m_displayCurve[i] ) { continue; }
            const auto& curve = m_curveControlPoints[i];
            for ( size_t j = 0; j < curve.size(); ++j )
            {
                const auto& P = curve[j];
                Scalar x      = zeroTime + P.x() * pixPerSec - event->x();
                Scalar y      = zeroValue - P.y() * pixPerVal - event->y();
                if ( x * x + y * y < CTRL_PT_RAD * CTRL_PT_RAD )
                {
                    m_currentControlPoint = {i, j};
                    m_mouseLeftClicked    = true;
                    event->accept();
                    return;
                }
            }
        }
        // move cursor without render
        if ( ctrlDown ) { onChangeCursor( newFrame, false ); }
        // delete KeyFrames between cursor and newCursor
        else if ( shiftDown )
        { deleteZone( m_cursor, newFrame ); }
        // move cursor and update renderer
        else
        {
            onChangeCursor( newFrame );
            m_mouseLeftClicked = true;
        }
    }
    // ------------------ RIGHT CLICK -------------------------------------
    else if ( event->button() == Qt::RightButton )
    {
        auto times = m_value->getTimes();
        auto it    = std::find_if( times.begin(), times.end(), [this]( const auto& t ) {
            return Ra::Core::Math::areApproxEqual( t, m_cursor );
        } );
        // if already on KeyFrame, move current KeyFrame
        // ------------------- CURSOR ON KEYFRAME -----------------------
        if ( it != times.end() )
        {
            Scalar nearest = nearestStep( newFrame );
            // -------------- SINGLE MOVE -------------------------------------
            if ( shiftDown )
            {
                // if no KeyFrame under mouse, move KeyFrame to newFrame
                auto it2 = std::find_if( times.begin(), times.end(), [nearest]( const auto& t ) {
                    return Ra::Core::Math::areApproxEqual( t, nearest );
                } );
                if ( it2 == times.end() && ( std::abs( m_cursor - nearest ) > 1e-5_ra ) )
                { onMoveKeyFrame( m_cursor, nearest ); }
            }
            // ---------- MULTIPLE MOVE -----------------------------------
            else
            {
                auto itLeft = it;
                --itLeft;
                Scalar left = ( it == times.begin() ) ? ( 0 ) : ( *itLeft );
                // if not before preceding KeyFrame, remove or insert time
                if ( nearest > left ) { onMoveKeyFrames( m_cursor, nearest - m_cursor ); }
            }
        }
        // ---------------- CURSOR NOT ON KEYFRAME --------------------------
        else
        {
            // if shiftdown, slide first right KeyFrame to the left
            // --------------- MOVE RIGHT KEYFRAME TO THE LEFT -----------------
            if ( shiftDown )
            {
                auto itRight = std::lower_bound( times.begin(), times.end(), newFrame );
                // if KeyFrames on the right, remove or insert time
                if ( itRight != times.end() ) { onMoveKeyFrames( *itRight, newFrame - *itRight ); }
            }
            // if not shiftdown, slide first left KeyFrame to the right
            // ---------------- MOVE LEFT KEYFRAME TO THE RIGHT -----------
            else
            {
                auto itLeft = --std::lower_bound( times.begin(), times.end(), newFrame );
                // if KeyFrames on the left, remove or insert time
                if ( itLeft != times.end() ) { onMoveKeyFrames( *itLeft, newFrame - *itLeft ); }
            }
        }
    }
    // no catch mouse event
    else
    {
        event->ignore();
        return;
    }
    event->accept();
}

#define insertVector( VECTOR_TYPE, N )                   \
    VECTOR_TYPE v;                                       \
    for ( uint i = 0; i < N; ++i )                       \
    {                                                    \
        v( i ) = m_curveControlPoints[i][uint( j )].y(); \
    }                                                    \
    kf->insertKeyFrame( time, v )

#define fetchMatrix()                                                    \
    Ra::Core::Matrix3 {                                                  \
        Ra::Core::AngleAxis( m_curveControlPoints[4][uint( j )].y(),     \
                             Ra::Core::Vector3::UnitX() ) *              \
            Ra::Core::AngleAxis( m_curveControlPoints[5][uint( j )].y(), \
                                 Ra::Core::Vector3::UnitY() ) *          \
            Ra::Core::AngleAxis( m_curveControlPoints[6][uint( j )].y(), \
                                 Ra::Core::Vector3::UnitZ() )            \
    }

void KeyFrameEditorFrame::mouseMoveEvent( QMouseEvent* event ) {
    if ( m_mouseLeftClicked )
    {
        int i = m_currentControlPoint.x();
        int j = m_currentControlPoint.y();
        if ( i != -1 && j != -1 )
        {
            Scalar value = ( -event->y() + m_ui->scrollArea->getZeroValue() ) /
                           m_ui->scrollArea->getPixPerValue();
            Scalar time = m_curveControlPoints[uint( i )][uint( j )].x();
            if ( auto kf = dynamic_cast<KeyFramedValue<bool>*>( m_value ) )
            {
                kf->insertKeyFrame( time, int( std::max( value, 0_ra ) ) );
                m_curveControlPoints[uint( i )][uint( j )].y() =
                    kf->at( time, linearInterpolate<bool> );
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<int>*>( m_value ) )
            {
                kf->insertKeyFrame( time, int( value ) );
                m_curveControlPoints[uint( i )][uint( j )].y() =
                    kf->at( time, linearInterpolate<int> );
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Scalar>*>( m_value ) )
            {
                kf->insertKeyFrame( time, value );
                m_curveControlPoints[uint( i )][uint( j )].y() =
                    kf->at( time, linearInterpolate<Scalar> );
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector2>*>( m_value ) )
            {
                insertVector( Ra::Core::Vector2, 2 );
                m_curveControlPoints[uint( i )][uint( j )].y() = value;
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector3>*>( m_value ) )
            {
                insertVector( Ra::Core::Vector3, 3 );
                m_curveControlPoints[uint( i )][uint( j )].y() = value;
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector4>*>( m_value ) )
            {
                insertVector( Ra::Core::Vector4, 4 );
                m_curveControlPoints[uint( i )][uint( j )].y() = value;
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Utils::Color>*>( m_value ) )
            {
                insertVector( Ra::Core::Utils::Color, 4 );
                m_curveControlPoints[uint( i )][uint( j )].y() = value;
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Quaternion>*>( m_value ) )
            {
                Ra::Core::Matrix3 R = fetchMatrix();
                kf->insertKeyFrame( time, Ra::Core::Quaternion( R ) );
                m_curveControlPoints[uint( i )][uint( j )].y() = value;
            }
            if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Transform>*>( m_value ) )
            {
                Ra::Core::Vector3 P( m_curveControlPoints[0][uint( j )].y(),
                                     m_curveControlPoints[1][uint( j )].y(),
                                     m_curveControlPoints[2][uint( j )].y() );
                Ra::Core::Matrix3 R = fetchMatrix();
                Ra::Core::Vector3 S( m_curveControlPoints[7][uint( j )].y(),
                                     m_curveControlPoints[8][uint( j )].y(),
                                     m_curveControlPoints[9][uint( j )].y() );
                Ra::Core::Transform T;
                T.fromPositionOrientationScale( P, R, S );
                kf->insertKeyFrame( time, T );
                m_curveControlPoints[uint( i )][uint( j )].y() = value;
            }
            update();
            emit keyFrameChanged( time );
        }
        else
        {
            Scalar newCursor = std::max( ( event->x() - m_ui->scrollArea->getZeroTime() ) /
                                             m_ui->scrollArea->getPixPerSec(),
                                         0_ra );

            onChangeCursor( newCursor );
        }
    }
    else
    { event->ignore(); }
}
#undef insertVector
#undef fetchMatrix

void KeyFrameEditorFrame::mouseReleaseEvent( QMouseEvent* event ) {
    if ( event->button() == Qt::LeftButton )
    {
        m_mouseLeftClicked    = false;
        m_currentControlPoint = {-1, -1};
        event->accept();
    }
    else
    { event->ignore(); }
}

Scalar KeyFrameEditorFrame::nearestStep( Scalar time ) const {
    Scalar deltaT = TIMELINE_AUTO_SUGGEST_CURSOR_RADIUS / m_ui->scrollArea->getPixPerSec();

    Scalar minDist = m_ui->scrollArea->getMaxTime();
    Scalar dist;

    Scalar newCursor = time;

    auto times = m_value->getTimes();
    for ( auto keyFrame : times )
    {
        dist = qAbs( keyFrame - time );
        if ( dist < deltaT && dist < minDist )
        {
            minDist   = dist;
            newCursor = keyFrame;
        }
        if ( time < keyFrame ) break;
    }

    const auto stepTime = m_ui->scrollArea->getStepTime();
    for ( int i = 0; i < m_ui->scrollArea->getNbIntervalTime() - 1; ++i )
    {
        Scalar pos = i * stepTime;
        dist       = qAbs( pos - time );
        if ( dist < deltaT && dist < minDist )
        {
            minDist   = dist;
            newCursor = pos;
        }

        pos  = i * stepTime + 0.5_ra * stepTime;
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

#define registerValue( type )                                                           \
    m_curveControlPoints[0].reserve( times.size() );                                    \
    for ( auto t : times )                                                              \
    {                                                                                   \
        m_curveControlPoints[0].push_back( {t, kf->at( t, linearInterpolate<type> )} ); \
    }

#define registerVector( N, type )                             \
    for ( size_t i = 0; i < N; ++i )                          \
    {                                                         \
        m_curveControlPoints[i].reserve( times.size() );      \
    }                                                         \
    for ( auto t : times )                                    \
    {                                                         \
        const auto f = kf->at( t, linearInterpolate<type> );  \
        for ( uint i = 0; i < N; ++i )                        \
            m_curveControlPoints[i].push_back( {t, f( i )} ); \
    }

void KeyFrameEditorFrame::registerKeyFrames( bool newValue ) {
    std::array<bool, 10> displayCurve = m_displayCurve;

    const auto display = [=]( QCheckBox* box, bool on ) {
        box->setChecked( on );
        on ? box->show() : box->hide();
    };

    if ( m_ui != nullptr )
    {
        display( m_ui->m_posX, false );
        display( m_ui->m_posY, false );
        display( m_ui->m_posZ, false );
        display( m_ui->m_posW, false );
        display( m_ui->m_scaleX, false );
        display( m_ui->m_scaleY, false );
        display( m_ui->m_scaleZ, false );
        display( m_ui->m_rotX, false );
        display( m_ui->m_rotY, false );
        display( m_ui->m_rotZ, false );
    }

    if ( m_value == nullptr ) { return; }
    // for each keyframe channel, collect the curve control points
    for ( auto& curve : m_curveControlPoints )
    {
        curve.clear();
    }

    auto times = m_value->getTimes();
    m_ui->m_nbKeyFrame->setText( QString::number( times.size() ) );
    if ( auto kf = dynamic_cast<KeyFramedValue<bool>*>( m_value ) )
    {
        m_curveControlPoints[0].reserve( times.size() );
        for ( auto t : times )
        {
            m_curveControlPoints[0].push_back(
                {t, ( kf->at( t, linearInterpolate<bool> ) ? 1 : 0 )} );
        }

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            m_ui->m_posX->setText( "x" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<int>*>( m_value ) )
    {
        registerValue( int );

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            m_ui->m_posX->setText( "x" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Scalar>*>( m_value ) )
    {
        registerValue( Scalar );

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            m_ui->m_posX->setText( "x" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector2>*>( m_value ) )
    {
        registerVector( 2, Ra::Core::Vector2 );

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            display( m_ui->m_posY, true );
            m_ui->m_posX->setText( "x" );
            m_ui->m_posY->setText( "y" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector3>*>( m_value ) )
    {
        registerVector( 3, Ra::Core::Vector3 );

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            display( m_ui->m_posY, true );
            display( m_ui->m_posZ, true );
            m_ui->m_posX->setText( "x" );
            m_ui->m_posY->setText( "y" );
            m_ui->m_posZ->setText( "z" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Vector4>*>( m_value ) )
    {
        registerVector( 4, Ra::Core::Vector4 );

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            display( m_ui->m_posY, true );
            display( m_ui->m_posZ, true );
            display( m_ui->m_posW, true );
            m_ui->m_posX->setText( "x" );
            m_ui->m_posY->setText( "y" );
            m_ui->m_posZ->setText( "z" );
            m_ui->m_posW->setText( "w" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Utils::Color>*>( m_value ) )
    {
        registerVector( 4, Ra::Core::Utils::Color );

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            display( m_ui->m_posY, true );
            display( m_ui->m_posZ, true );
            display( m_ui->m_posW, true );
            m_ui->m_posX->setText( "r" );
            m_ui->m_posY->setText( "g" );
            m_ui->m_posZ->setText( "b" );
            m_ui->m_posW->setText( "a" );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Quaternion>*>( m_value ) )
    {
        m_curveControlPoints[4].reserve( times.size() );
        m_curveControlPoints[5].reserve( times.size() );
        m_curveControlPoints[6].reserve( times.size() );
        for ( auto t : times )
        {
            const auto f = kf->at( t, linearInterpolate<Ra::Core::Quaternion> )
                               .matrix()
                               .eulerAngles( 0, 1, 2 );
            m_curveControlPoints[4].push_back( {t, f.x()} );
            m_curveControlPoints[5].push_back( {t, f.y()} );
            m_curveControlPoints[6].push_back( {t, f.z()} );
        }

        if ( m_ui != nullptr )
        {
            display( m_ui->m_rotX, true );
            display( m_ui->m_rotY, true );
            display( m_ui->m_rotZ, true );
        }
    }
    else if ( auto kf = dynamic_cast<KeyFramedValue<Ra::Core::Transform>*>( m_value ) )
    {
        for ( size_t i = 0; i < 10; ++i )
        {
            m_curveControlPoints[i].reserve( times.size() );
        }
        for ( auto t : times )
        {
            const auto f = kf->at( t, linearInterpolate<Ra::Core::Transform> );
            const auto T = f.translation();
            Ra::Core::Matrix3 R, S;
            f.computeRotationScaling( &R, &S );
            auto angles = R.eulerAngles( 0, 1, 2 );
            m_curveControlPoints[0].push_back( {t, T( 0 )} );
            m_curveControlPoints[1].push_back( {t, T( 1 )} );
            m_curveControlPoints[2].push_back( {t, T( 2 )} );
            m_curveControlPoints[4].push_back( {t, angles.x()} );
            m_curveControlPoints[5].push_back( {t, angles.y()} );
            m_curveControlPoints[6].push_back( {t, angles.z()} );
            m_curveControlPoints[7].push_back( {t, S( 0, 0 )} );
            m_curveControlPoints[8].push_back( {t, S( 1, 1 )} );
            m_curveControlPoints[9].push_back( {t, S( 2, 2 )} );
        }

        if ( m_ui != nullptr )
        {
            display( m_ui->m_posX, true );
            display( m_ui->m_posY, true );
            display( m_ui->m_posZ, true );
            display( m_ui->m_rotX, true );
            display( m_ui->m_rotY, true );
            display( m_ui->m_rotZ, true );
            display( m_ui->m_scaleX, true );
            display( m_ui->m_scaleY, true );
            display( m_ui->m_scaleZ, true );
            m_ui->m_posX->setText( "x" );
            m_ui->m_posY->setText( "y" );
            m_ui->m_posZ->setText( "z" );
        }
    }
    else
    { LOG( logWARNING ) << "[KeyFrameEditor] Unknown KeyFrame type, cannot edit."; }

    if ( !newValue )
    {
        m_displayCurve = displayCurve;
        m_ui->m_posX->setChecked( m_displayCurve[0] );
        m_ui->m_posY->setChecked( m_displayCurve[1] );
        m_ui->m_posZ->setChecked( m_displayCurve[2] );
        m_ui->m_posW->setChecked( m_displayCurve[3] );
        m_ui->m_rotX->setChecked( m_displayCurve[4] );
        m_ui->m_rotY->setChecked( m_displayCurve[5] );
        m_ui->m_rotZ->setChecked( m_displayCurve[6] );
        m_ui->m_scaleX->setChecked( m_displayCurve[7] );
        m_ui->m_scaleY->setChecked( m_displayCurve[8] );
        m_ui->m_scaleZ->setChecked( m_displayCurve[9] );
    }
}
#undef registerValue
#undef registerValue

void KeyFrameEditorFrame::updateCursorSpin() {
    if ( m_ui == nullptr ) { return; }
    auto times = m_value->getTimes();
    auto it    = std::find_if( times.begin(), times.end(), [this]( const auto& t ) {
        return Ra::Core::Math::areApproxEqual( t, m_cursor );
    } );
    if ( it != times.end() )
    {
        m_ui->m_currentTime_dsb->setStyleSheet( "background-color: yellow" );
        m_ui->m_deleteKeyframe->setEnabled( true );
    }
    else
    {
        m_ui->m_currentTime_dsb->setStyleSheet( "background-color: #5555ff" );
        m_ui->m_deleteKeyframe->setEnabled( false );
    }
    m_ui->m_currentTime_dsb->setValue( double( m_cursor ) );
}

} // namespace Ra::GuiBase
