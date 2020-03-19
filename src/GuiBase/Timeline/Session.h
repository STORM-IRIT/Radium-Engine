#ifndef RADIUMENGINE_TIMELINE_SESSION_SESSION_H_
#define RADIUMENGINE_TIMELINE_SESSION_SESSION_H_

#include <set>
#include <stack>

#include <QTimer>

namespace Ra::GuiBase {
class QDoubleSpinBoxSmart;
class QFrameSelector;
class QSpinBoxSmart;
class QToolButtonPlayPause;
class QWidgetRuler;
} // namespace Ra::GuiBase

namespace Ra::GuiBase {

class Session : public QObject
{
    Q_OBJECT
  public:
    Session( QObject* parent = nullptr );
    virtual ~Session();

    void setStart( double* value );
    void setEnd( double* value );
    void setCursor( double* value );
    void setDuration( double* value );
    void setKeyPoses( std::set<double>* value );
    void setStartSpin( QDoubleSpinBoxSmart* value );
    void setEndSpin( QDoubleSpinBoxSmart* value );
    void setCursorSpin( QDoubleSpinBoxSmart* value );
    void setDurationSpin( QDoubleSpinBoxSmart* value );
    void setPlayButton( QToolButtonPlayPause* value );
    void setRuler( QWidgetRuler* value );
    void setSelector( QFrameSelector* value );
    void setNbKeyPosesSpin( QSpinBoxSmart* value );

  signals:
    void envSaved(); // EXTERNAL

    void rendered( void* render );      // EXTERNAL
    void renderDeleted( void* render ); // EXTERNAL

  public slots:
    void onChangeEnv();

    void onClearSession();
    void onUndo();
    void onRedo();

    void onSaveRendering( void* anim, size_t bytes ); // EXTERNAL

  private:
    typedef struct s_Env {
        double m_start;
        double m_end;
        double m_cursor;
        double m_duration;

        std::set<double> m_keyPoses;

        void* m_anim;
        size_t m_bytes;
    } Env;

  private:
    void setEnv( Env env );

  private:
    std::deque<Env> m_undo;
    std::stack<Env> m_redoHeap;

    Env m_first;

    double* m_start;
    double* m_end;
    double* m_cursor;
    double* m_duration;
    std::set<double>* m_keyPoses;

    size_t m_size{0};

    QDoubleSpinBoxSmart* m_startSpin;
    QDoubleSpinBoxSmart* m_endSpin;
    QDoubleSpinBoxSmart* m_cursorSpin;
    QDoubleSpinBoxSmart* m_durationSpin;

    QToolButtonPlayPause* m_playButton;

    QWidgetRuler* m_ruler;

    QTimer* m_saveDelay;
    QFrameSelector* m_selector;
    QSpinBoxSmart* m_nbKeyPosesSpin;
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_TIMELINE_SESSION_SESSION_H_
