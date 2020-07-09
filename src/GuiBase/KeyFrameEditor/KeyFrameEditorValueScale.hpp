#pragma once

#include <QFrame>

namespace Ra::GuiBase {
class KeyFrameEditorScrollArea;
} // namespace Ra::GuiBase

namespace Ra::GuiBase {

/**
 * The KeyframeEditorValueScale class displays the Value scale, along the y-axis,
 * of the KeyframeEditorScrollArea it belongs to.
 */
class KeyframeEditorValueScale : public QFrame
{
    Q_OBJECT
  public:
    explicit KeyframeEditorValueScale( QWidget* parent = nullptr );

    /// Set the KeyframeEditorScrollArea for which to display the scale.
    void setScrollArea( KeyFrameEditorScrollArea* scrollArea );

  protected:
    virtual void paintEvent( QPaintEvent* event ) override;

  private:
    /// The KeyframeEditorScrollArea.
    KeyFrameEditorScrollArea* m_scrollArea {nullptr};
};

} // namespace Ra::GuiBase
