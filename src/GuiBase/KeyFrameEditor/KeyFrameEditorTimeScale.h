#ifndef RADIUMENGINE_KEY_FRAME_EDITOR_TIMESCALE_H
#define RADIUMENGINE_KEY_FRAME_EDITOR_TIMESCALE_H

#include <QFrame>

namespace Ra::GuiBase {
class KeyFrameEditorScrollArea;
} // namespace Ra::GuiBase

namespace Ra::GuiBase {

/**
 * The KeyFrameEditorTimeScale class displays the scale, along the x-axis,
 * of the KeyFrameEditorScrollArea it belongs to.
 */
class KeyFrameEditorTimeScale : public QFrame
{
    Q_OBJECT
  public:
    explicit KeyFrameEditorTimeScale( QWidget* parent = nullptr );

    /// Set the KeyFrameEditorScrollArea for which to display the scale.
    void setScrollArea( KeyFrameEditorScrollArea* scrollArea );

  protected:
    virtual void paintEvent( QPaintEvent* event ) override;

  private:
    /// The KeyFrameEditorScrollArea.
    KeyFrameEditorScrollArea* m_scrollArea{nullptr};
};

} // namespace Ra::GuiBase

#endif // RADIUMENGINE_KEY_FRAME_EDITOR_TIMESCALE_H
