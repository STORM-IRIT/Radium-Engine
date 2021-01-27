#pragma once

#include <QScrollArea>

#include <Core/CoreMacros.hpp>

namespace Ra::GuiBase {

/**
 * The KeyFrameEditorScrollArea class manages the ScrollArea for the KeyFrameEditor.
 *
 * It offers display info for its sub-widgets (size, zoom level, scroll shift...).
 */
class KeyFrameEditorScrollArea : public QScrollArea
{
    Q_OBJECT
  public:
    explicit KeyFrameEditorScrollArea( QWidget* parent = nullptr );

    /// Set the <b>end<\b> of the Timeline's playzone.
    void setMaxTime( Scalar maxTime );

    /// \returns the <b>end<\b> of the Timeline's playzone.
    Scalar getMaxTime();

    /// \return the step between two time scale graduations.
    Scalar getStepTime();

    /// \returns the number of time scale graduations.
    int getNbIntervalTime();

    /// \returns the number of pixels used to display 1 second on the time scale.
    Scalar getPixPerSec();

    /// \returns the pixel corresponding to time 0.
    Scalar getZeroTime();

    /// Return the maximal value of the edited KeyFramedValue.
    Scalar getMaxValue();

    /// \return the step between two value scale graduations.
    Scalar getStepValue();

    /// \returns the number of value scale graduations.
    int getNbIntervalValue();

    /// \returns the number of pixels used to display 1 second on the value scale.
    Scalar getPixPerValue();

    /// \returns the pixel corresponding to value 0.
    Scalar getZeroValue();

  signals:
    /// Emitted when zoomed in/out or resized.
    void stepChanged( Scalar step );

    /// Emitted when <i> is pressed.
    void addKeyFrame();

    /// Emitted when <del> / <shift>+<i> is pressed.
    void removeKeyFrame();

    /// Emitted when <left_arrow> is pressed or <shift> + <wheel-up>.
    void previousKeyFrame();

    /// Emitted when <right_arrow> is pressed or <shift> + <wheel-down>.
    void nextKeyFrame();

    /// Emitted when <space> is pressed.
    void togglePlayPause();

    /// Emitted when <crtl> + <z> / <u> key is pressed.
    void undo();

    /// Emitted when <crtl> + <shift> + <z> / <r> key is pressed.
    void redo();

  public slots:
    /// Force redraw with the given width.
    void onDrawRuler( int width, int height );

  protected:
    void keyPressEvent( QKeyEvent* event ) override;

    void wheelEvent( QWheelEvent* event ) override;

    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;

  private:
    int m_mousePosX;  ///< x coordinate of the mouse on mouse middle click.
    int m_mousePosY;  ///< y coordinate of the mouse on mouse middle click.
    int m_sliderPosX; ///< x coordinate of the slider on mouse middle click.
    int m_sliderPosY; ///< y coordinate of the slider on mouse middle click.

    Scalar m_maxTime {200};   ///< <b>end<\b> of the Timeline's playzone.
    Scalar m_stepTime {10};   ///< Step between two time scale graduations.
    int m_nbIntervalTime {0}; ///< Number of time scale graduations.
    Scalar m_pixPerTime {10}; ///< Number of pixels used to display 1 second on the scale.
    Scalar m_zeroTime {10};   ///< Pixel corresponding to time 0.

    Scalar m_maxValue {200};   ///< Maximal extent of a KeyFrame value.
    Scalar m_stepValue {10};   ///< Step between two value scale graduations.
    int m_nbIntervalValue {0}; ///< Number of value scale graduations.
    Scalar m_pixPerValue {
        10}; ///< Number of pixels used to display a value difference of 1 on the scale.
    Scalar m_zeroValue {10}; ///< Pixel corresponding to time 0.

    /// Number of possible steps.
    static constexpr int s_nbSteps {13};

    /// Possible steps.
    static constexpr Scalar s_steps[s_nbSteps] {0.01_ra,
                                                0.02_ra,
                                                0.05_ra,
                                                0.1_ra,
                                                0.2_ra,
                                                0.5_ra,
                                                1.0_ra,
                                                2.0_ra,
                                                5.0_ra,
                                                10.0_ra,
                                                20.0_ra,
                                                50.0_ra,
                                                100.0_ra};
};

} // namespace Ra::GuiBase
