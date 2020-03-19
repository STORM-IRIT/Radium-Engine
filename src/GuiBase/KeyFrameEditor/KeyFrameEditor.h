#ifndef RADIUMENGINE_KEYFRAME_EDITOR_H
#define RADIUMENGINE_KEYFRAME_EDITOR_H

#include <GuiBase/RaGuiBase.hpp>

#include <map>
#include <vector>

#include <QDialog>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>

#include <Core/CoreMacros.hpp>

namespace Ra::Core::Animation {
class KeyFramedValueBase;
}

namespace Ui {
class KeyFrameEditor;
} // namespace Ui

namespace Ra::GuiBase {

class RA_GUIBASE_API KeyFrameEditor : public QDialog
{
    Q_OBJECT
  public:
    using KeyFrame = Ra::Core::Animation::KeyFramedValueBase;
    explicit KeyFrameEditor( Scalar maxTime = 0, QWidget* parent = nullptr );
    ~KeyFrameEditor() override;

    /// Set the KeyFramedValue to edit.
    void setKeyFramedValue( const std::string& name, KeyFrame* frame );

  signals:
    /// Emitted when the user changes the current time in the editor frame.
    void cursorChanged( Scalar time );

    /// Emitted when the user changes a keyframe's value in the editor frame.
    void keyFrameChanged( size_t i );

    /// Emitted when the user adds a keyframe to the KeyFramedValue in the editor frame.
    void keyFrameAdded( Scalar time );

    /// Emitted when the user removes a keyframe from the KeyFramedValue in the editor frame.
    void keyFrameDeleted( size_t i );

    /// Emitted when the user changes a keyframe's time in the editor frame.
    void keyFrameMoved( size_t i, Scalar time1 );

    /// Emitted when the user offsets keyframes time in the editor frame.
    void keyFramesMoved( size_t first, Scalar offset );

  public slots:
    /// Update the editor frame to match the given time.
    void onChangeCursor( Scalar time );

    /// Update the editor frame to match the given duration.
    void onChangeDuration( Scalar duration );

    /// Update the editor frame to match the updated KeyFramedValue.
    void onUpdateKeyFrames( Scalar currentTime );

  protected:
    void resizeEvent( QResizeEvent* ev ) override;

  private:
    Ui::KeyFrameEditor* ui;
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_KEYFRAME_EDITOR_H
