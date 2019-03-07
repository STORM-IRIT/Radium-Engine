#ifndef RADIUMENGINE_COLORWIDGET_HPP
#define RADIUMENGINE_COLORWIDGET_HPP

#include <QWidget>

namespace Ra {
namespace Gui {

/**
 * The ColorWidget class is a simple widget allowing the selection
 * of a color when clicking it.
 */
class ColorWidget : public QWidget {
    Q_OBJECT

  public:
    ColorWidget( QWidget* parent = nullptr );

  public slots:
    /**
     * Updates the widget color according to to new color.
     */
    void colorChanged( int r, int g, int b );

  signals:
    /**
     * Emitted when a new color has been picked.
     */
    void newColorPicked( const QColor& color );

  private:
    void mousePressEvent( QMouseEvent* e ) override;

    /**
     * Updates the widget color according to to new color.
     */
    void colorChanged();

  private:
    /// The current color.
    QColor m_currentColor;
};

} // namespace Gui
} // namespace Ra

#endif
