#ifndef RADUIUMENGINE_QFRAMESELECTOR_H_
#define RADUIUMENGINE_QFRAMESELECTOR_H_

#include <GuiBase/Timeline/Configurations.h>

#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QSpacerItem>
#include <QToolButton>
#include <set>

namespace Ra::GuiBase {
class QWidgetRuler;
}

namespace Ra::GuiBase {

class QFrameSelector : public QFrame
{
    Q_OBJECT
  public:
    explicit QFrameSelector( QWidget* parent = nullptr );
    ~QFrameSelector() override;

    //    void updatePlayZone();

    double nearestStep( double time ) const;

  signals:
    void cursorChanged( double time );   // EXTERNAL SIGNAL
    void startChanged( double time );    // EXTERNAL SIGNAL
    void endChanged( double time );      // EXTERNAL SIGNAL
    void durationChanged( double time ); // EXTERNAL SIGNAL

    void keyPoseAdded( double time );               // EXTERNAL SIGNAL
    void keyPoseDeleted( size_t id );               // EXTERNAL SIGNAL
    void keyPoseChanged( size_t id );               // EXTERNAL SIGNAL
    void keyPosesMoved( double gap, size_t first ); // EXTERNAL SIGNAL
    void keyPoseMoved( size_t id, double time );    // EXTERNAL SIGNAL

  public slots:
    // ---------------------- EXTERNAL SLOTS ----------------------------------
    void onAddingKeyPose( double time = -1.0, bool internal = true ); // EXTERNAL SLOT
    void onClearKeyPoses();                                           // EXTERNAL SLOT

    void onChangeStart( double time, bool internal = true );    // EXTERNAL SLOT
    void onChangeEnd( double time, bool internal = true );      // EXTERNAL SLOT
    void onChangeCursor( double time, bool internal = true );   // EXTERNAL SLOT
    void onChangeDuration( double time, bool internal = true ); // EXTERNAL SLOT

    //
    // ---------------------- INTERNAL SLOTS ----------------------------------
    void onSlideLeftSlider( int deltaX );
    void onSlideRightSlider( int deltaX );
    void onLeftSlideRelease();
    void onRightSlideRelease();

    void onDeleteKeyPose();

    void onSetCursorToStart();
    void onSetCursorToEnd();
    void onSetCursorToPreviousKeyPose();
    void onSetCursorToNextKeyPose();

    void onChangeStartSpin();
    void onChangeEndSpin();
    void onChangeCursorSpin();
    void onChangeDurationSpin();

    void updateCursorSpin();
    void updateStartSpin();
    void updateEndSpin();
    void updateDurationSpin();
    void redrawPlayZone();

  public:
    // --------------------------- GETTERS ------------------------------------
    double getStart() const;
    double* getStart();
    double* getEnd();
    double* getCursor();
    double getEnd() const;
    double getCursor() const;
    int getNbKeyPoses() const;
    double getKeyPose( int id ) const;
    std::set<double> getKeyPoses() const;
    std::set<double>* getKeyPoses();

    // --------------------------- SETTERS ------------------------------------
    void setCursor( double time );
    void setKeyPoses( const std::set<double>& value );
    void setShiftDown( bool* value );
    void setStart( double value );
    void setEnd( double value );
    void setDuration( double time );

    //
    // ---------------------- REFERENCES SETTERS ------------------------------
    void setLeftSpacer( QFrame* value );
    void setLeftSlider( QLabel* value );
    void setPlayZone( QFrame* value );
    void setRightSlider( QLabel* value );

    void setCursorSpin( QDoubleSpinBox* value );
    void setStartSpin( QDoubleSpinBox* value );
    void setEndSpin( QDoubleSpinBox* value );
    void setDurationSpin( QDoubleSpinBox* value );

    void setRemoveKeyPoseButton( QToolButton* value );
    void setStartInc( QDoubleSpinBox* value );
    void setEndInc( QDoubleSpinBox* value );
    void setNbKeyPosesSpin( QSpinBox* value );
    void setMidMouseDown( bool* value );
    void setCtrlDown( bool* value );

  protected:
    virtual void paintEvent( QPaintEvent* event ) override;

    void mousePressEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;

  private:
    // ---------------------- PRIVATE FUNCTIONS --------------------------------
    void moveKeyPoses( double gap, size_t first = 0 );
    void deleteZone( double time, double time2 );

  private:
    int m_paintCounter{0};

    double m_start;
    double m_end;
    double m_cursor;

    int* m_nbInterval;
    double* m_step;
    double* m_pixPerSec;
    double* m_zero;
    double* m_duration;

    // question : why QSet is unordered
    //    QSet<double> keyPoses;
    std::set<double> m_keyPoses;

    bool m_sliding{false};
    bool m_mouseLeftClicked{false};

    bool* m_midMouseDown;

    bool* m_shiftDown;
    bool* m_ctrlDown;

    int m_updateKeyPoseFlash{0};
    double m_keyPoseFlash;

    QTimer* m_timer;

    // ---------------------- REFERENCES --------------------------------------
    QWidgetRuler* m_widgetRuler;

    QFrame* m_leftSpacer;
    QLabel* m_leftSlider;
    QFrame* m_playZone;
    QLabel* m_rightSlider;

    QDoubleSpinBox* m_cursorSpin;
    QDoubleSpinBox* m_startSpin;
    QDoubleSpinBox* m_endSpin;
    QDoubleSpinBox* m_durationSpin;

    QToolButton* m_removeKeyPoseButton;
    QDoubleSpinBox* m_startInc;
    QDoubleSpinBox* m_endInc;
    QSpinBox* m_nbKeyPosesSpin;
};

} // namespace Ra::GuiBase

#endif // RADUIUMENGINE_QFRAMESELECTOR_H_
