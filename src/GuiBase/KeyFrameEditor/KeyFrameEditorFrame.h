#ifndef RADIUMENGINE_KEYFRAME_EDITOR_FRAME_H
#define RADIUMENGINE_KEYFRAME_EDITOR_FRAME_H

#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QSpacerItem>
#include <QToolButton>
#include <set>

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra::Core::Animation {
class KeyFramedValueBase;
}

namespace Ra::GuiBase {
class KeyFrameEditorScrollArea;
}

namespace Ui {
class KeyFrameEditor;
}

namespace Ra::GuiBase {
class KeyFrameEditor;

/**
 * The KeyFrameEditorFrame manages the display of keyframed values in the KeyFrameEditor.
 * It also manages the update of the KeyFrameEditor UI.
 */
class KeyFrameEditorFrame : public QFrame
{
    Q_OBJECT
  public:
    friend class KeyFrameEditor;
    /// Shorthand for KeyFramedValue.
    using KeyFrame = Ra::Core::Animation::KeyFramedValueBase;

    explicit KeyFrameEditorFrame( QWidget* parent = nullptr );
    ~KeyFrameEditorFrame() override;

    /// Set the Editor main UI for posterior update.
    void setEditorUi( Ui::KeyFrameEditor* ui );

    /// Set the duration of the KeyFrameEditor's playzone.
    void setDuration( Scalar time );

    /// Registers the KeyFramedValue begin edited.
    void setKeyFramedValue( KeyFrame* frame );

    /**
     * Return the set of points in time where a KeyFrame is defined for the
     * currently edited KeyFramedValue.
     */
    std::set<Scalar> getKeyFrames() const;

  public slots:
    /**
     * Set the current time of the KeyFrameEditor's playzone to the given time.
     * \param time the new current time.
     * \param internal whether the call has been made internally by the KeyFrameEditor,
     *        e.g. through the UI; or externally through KeyFrameEditor::onChangeEnd()
     *        or through the TimelineFrameSelector.
     * \note If \p internal is set to true, the signal cursorChanged() will be
     *       emitted.
     */
    void onChangeCursor( Scalar time, bool internal = true );

    /**
     * Set the current time to the given time and update the display of the
     * edited KeyFramedValue at the given time.
     */
    void onUpdateKeyFrames( Scalar currentTime );

  signals:
    /**
     * Emitted when the current time has been changed though
     *  - the user changing the current time in the KeyFrameEditor,
     *  - the user offsetting a set of KeyFrames,
     *  - the user deleting an entire timezone.
     */
    void cursorChanged( Scalar time );

    /// Emitted when the user changes a KeyFrame's value.
    void keyFrameChanged( Scalar time );

    /// Emitted when the user adds a KeyFrame to the KeyFramedValue.
    void keyFrameAdded( Scalar time );

    /**
     * Emitted when a KeyFrame has been deleted through
     *  - the user deleting a single KeyFrame.
     *  - the user deleting an entire timezone containing a KeyFrame.
     * \note Emitted for each suppressed KeyFrame.
     */
    void keyFrameDeleted( Scalar time );

    /// Emitted when the user changes a KeyFrame's time.
    void keyFrameMoved( Scalar time0, Scalar time1 );

    /**
     * Emitted when a KeyFrame has been added through
     *  - the user offsetting a set of keyframes.
     *  - the user deleting an entire timezone preceding KeyFrames.
     */
    void keyFramesMoved( Scalar time, Scalar offset );

    /// Emitted when the editor has been updated.
    void updated();

  private slots:
    /**
     * Emits keyFrameAdded() and then calls onUpdateKeyFrames().
     * \note If there already is a Keyframe at the current time, does nothing.
     */
    void onAddKeyFrame();

    /**
     * Emits keyFrameDeleted() and then calls onUpdateKeyFrames().
     * \note If there is no Keyframe at the current time, does nothing.
     */
    void onDeleteKeyFrame();

    /**
     * Emits keyFrameMoved() and then calls onUpdateKeyFrames().
     * \note If \p time0 == \p time1, does nothing.
     */
    void onMoveKeyFrame( Scalar time0, Scalar time1 );

    /**
     * Emits keyFramesMoved() and then calls onUpdateKeyFrames().
     * \note If \p offset == 0, or there is no KeyFrame at \p time, does nothing.
     * \note If \p offset is negative and offsetting the KeyFrame at \p time covers
     *       the current time, this one will be offset too and cursorChanged() will
     *       be emitted.
     */
    void onMoveKeyFrames( Scalar time, Scalar offset );

    /**
     * Emits keyFrameMoved for each KeyFrame after \p time2,
     * keyFrameDeleted for each KeyFrame between time and time2,
     * then emits cursorChanged then calls onUpdateKeyFrames().
     */
    void deleteZone( Scalar time, Scalar time2 );

    /**
     * Calls onChangeCursor() with the time of the KeyFrame right before m_cursor
     * if it exists, does nothing otherwise.
     */
    void onSetCursorToPreviousKeyFrame();

    /**
     * Calls onChangeCursor() with the time of the KeyFrame right after m_cursor
     * if it exists, does nothing otherwise.
     */
    void onSetCursorToNextKeyFrame();

  protected:
    virtual void paintEvent( QPaintEvent* event ) override;

    void mousePressEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;

  private:
    /**
     * Tries to stick the cursor from \p time to the nearest KeyFrame,
     * depending on the zoom level.
     * \returns the nearest KeyFrame if could stick, time otherwise.
     */
    Scalar nearestStep( Scalar time ) const;

    /**
     * Registers the KeyFramed value channels for display update.
     * \param newValue whether the Keyframed value has changed or not.
     */
    void registerKeyFrames( bool newValue = false );

    /// Updates the "cursor" SpinBox.
    void updateCursorSpin();

  private:
    /// The currently edited KeyFramedValue.
    KeyFrame* m_value{nullptr};

    /// The current time of the Timeline's playzone.
    Scalar m_cursor;

    /// The display state of the KeyFramed value channels.
    std::array<bool, 10> m_displayCurve =
        {true, true, true, true, true, true, true, true, true, true};

    /// Whether the user is editing values or not.
    bool m_mouseLeftClicked{false};

    /// The type for KeyFramedValue edition control points.
    using CurveControlPoints = std::vector<Ra::Core::Vector2>;

    /// The per-channel sets of control points.
    std::array<CurveControlPoints, 10> m_curveControlPoints;

    /// The current control point.
    Ra::Core::Vector2i m_currentControlPoint{-1, -1};

    /// The QPainter to display all the data.
    QPainter* m_painter{nullptr};

    /// The KeyFrameEditor UI.
    Ui::KeyFrameEditor* m_ui{nullptr};
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_KEYFRAME_EDITOR_FRAME_H
