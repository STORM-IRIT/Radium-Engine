#pragma once

#include <QFrame>

namespace Ra::GuiBase {
class KeyFrameEditorFrame;
class KeyFrameEditorScrollArea;
} // namespace Ra::GuiBase

namespace Ra::GuiBase {

/**
 * The KeyFrameEditorFrameScale class displays, along the x-axis,
 * the KeyFrames for the KeyFrameEditorFrame it belongs to.
 */
class KeyFrameEditorFrameScale : public QFrame
{
    Q_OBJECT
  public:
    explicit KeyFrameEditorFrameScale( QWidget* parent = nullptr );

    /// Set the KeyFrameEditorFrame for which to display the KeyFrames.
    void setEditorFrame( KeyFrameEditorFrame* editorFrame );

    /// Set the KeyFrameEditorScrollArea to display the KeyFrames in.
    void setScrollArea( KeyFrameEditorScrollArea* scrollArea );

  protected:
    virtual void paintEvent( QPaintEvent* event ) override;

  private:
    /// The KeyFrameEditorFrame.
    KeyFrameEditorFrame* m_editorFrame {nullptr};

    /// The KeyFrameEditorScrollArea.
    KeyFrameEditorScrollArea* m_scrollArea {nullptr};
};

} // namespace Ra::GuiBase
