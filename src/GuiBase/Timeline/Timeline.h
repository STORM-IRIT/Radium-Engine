/*!
 * \brief Custom Qt widget for animation (timeline)
 * \author Gauthier Bouyjou (email : gauthierbouyjou@aol.com)
 * \date april 2019
 *
 * For question send me mail or add issue to initial
 * github repo : https://github.com/hiergaut/AnimTimeline.git
 * current commit id : aaa363ee8ec0247e9299a8949e5bd334b10dc413
 */

#ifndef RADIUMENGINE_TIMELINE_H_
#define RADIUMENGINE_TIMELINE_H_

#include <GuiBase/Timeline/Session.h>

#include <QDialog>
#include <QObject>

namespace Ui {
class Timeline;
}

namespace Ra::GuiBase {

/*!
 * \brief The Timeline class is a minimal specification of timeline abilities
 */
class Timeline : public QDialog
{
    Q_OBJECT

  public:
    explicit Timeline( QWidget* parent = nullptr );
    ~Timeline() override;

  protected:
    virtual void resizeEvent( QResizeEvent* ev ) override;

  signals:
    /*!
     * \brief startChanged is emitted when user move left slider of playzone or set new value in
     * start spin (green) \param time is the new start time for playzone
     */
    void startChanged( double time );
    /*!
     * \brief endChanged is emitted when user move right slider of playzone or set new value in end
     * spin (red) \param time is the new end time for playzone
     */
    void endChanged( double time );
    /*!
     * \brief cursorChanged is emitted when user move cursor
     * \param time is the new time of cursor to render in engine
     */
    void cursorChanged( double time );
    /*!
     * \brief durationChanged is emitted when user change duration time in top right spin
     * \param time is the new time for animation
     */
    void durationChanged( double time );

    /*!
     * \brief keyPoseAdded is emitted when user add new keyPose
     * \param time is the time of new keyPose, if a keyPose is already here so signal keyPoseChanged
     * is called
     */
    void keyPoseAdded( double time );
    /*!
     * \brief keyPoseDeleted is emitted when user remove keyPose
     * \param id is the ith keyPose to remove (chronological order)
     */
    void keyPoseDeleted( size_t id );
    /*!
     * \brief keyPoseChanged is emitted when user insert keyPose in a known keyPose position
     * \param id is the ith keyPose to change
     */
    void keyPoseChanged( size_t id );
    /*!
     * \brief keyPoseMoved is emitted when user move keyPose on cursor to new position (on mouse)
     * \param id is the ith keyPose to move
     * \param time is the time of keyPose, move current keyPose in other keyPose is not possible
     */
    void keyPoseMoved( size_t id, double time );
    /*!
     * \brief keyPosesMoved is emitted when user move keyPoses
     * \param gap is the sliding distance for moving
     * \param first is the first keyPose to move with its right brothers
     */
    void keyPosesMoved( double gap, size_t first = 0 );

    /*!
     * \brief playClicked is emitted when user click on play button
     */
    void playClicked();
    /*!
     * \brief pauseClicked is emitted when user click on pause button
     */
    void pauseClicked();

  public slots:
    /*!
     * \brief onChangeStart change start in timeline
     * \param time is the new start
     */
    virtual void onChangeStart( double time );
    /*!
     * \brief onChangeEnd change end in timeline
     * \param time is the new end
     */
    virtual void onChangeEnd( double time );
    /*!
     * \brief onChangeCursor change cursor in timeline
     * \param time is the new cursor
     */
    virtual void onChangeCursor( double time );
    /*!
     * \brief onChangeDuration change duration in timeline
     * \param time is the new duration
     */
    virtual void onChangeDuration( double time );

    /*!
     * \brief onAddingKeyPose add keyPose in timeline
     * \param time is the new keyPose time
     */
    virtual void onAddingKeyPose( double time );
    /*!
     * \brief onClearKeyPoses remove all keyPoses in timeline
     */
    virtual void onClearKeyPoses();

    /*!
     * \brief onSetPlayMode set play mode in timeline
     */
    virtual void onSetPlayMode(); // use it if external play button
    /*!
     * \brief onSetPauseMode set pause mode in timeline
     */
    virtual void onSetPauseMode(); // use it if external pause button

  protected:
    Ui::Timeline* ui;
};

/*!
 * \brief The TimelineWithSession class is a Timeline with session, permit (undo/redo)
 */
class TimelineWithSession : public Timeline
{
    Q_OBJECT

  public:
    explicit TimelineWithSession( QWidget* parent = nullptr );

  signals:
    /*!
     * \brief envSaved is emitted for session, after receive this signal, user must call
     * onSaveRendering slot of Timeline to save client anim to render later due of undo/redo envent
     */
    void envSaved();
    /*!
     * \brief rendered is emmited when undo/redo envent is calling and need to render previous
     * environment \param is the environment to render, anim is void* because Qt Q_OBJECT no accept
     * template class and signal and slots are needed to send signal to user to render
     */
    void rendered( void* anim );
    /*!
     * \brief renderDeleted is emmited when environment is not needed for future
     * \param anim is the environment to delete
     */
    void renderDeleted( void* anim );

  public slots:
    /*!
     * \brief onSaveRendering must be called by user after envSaved receive
     * \param anim is the minimal environment to save in timeline session to render later
     * \param bytes is the size of minimal environment, permit to save RAM
     */
    void onSaveRendering( void* anim, size_t bytes );

  public slots:
    void onChangeStart( double time ) override;
    void onChangeEnd( double time ) override;
    //    void onChangeCursor(double time);
    void onChangeDuration( double time ) override;

    void onAddingKeyPose( double time ) override;
    void onClearKeyPoses() override;

    //    void onSetPlayMode();
    //    void onSetPauseMode();

  private:
    Session session{this};
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_TIMELINE_H_
