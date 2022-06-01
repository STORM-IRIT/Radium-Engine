#pragma once

#ifdef ENABLE_QDOM
#    include <QDomElement>
#endif
#include <QWidget>

#include <Gui/RaGui.hpp>

#ifdef POWERSLIDER_DESIGNER_PLUGIN
#    include <QtUiPlugin/QDesignerExportWidget>
#else
#    define QDESIGNER_WIDGET_EXPORT
#endif

class QSlider;
class QDoubleSpinBox;

namespace Ra::Gui::Widgets {

class QDESIGNER_WIDGET_EXPORT RA_GUI_API PowerSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( double Min READ minimum WRITE setMinimum )
    Q_PROPERTY( double Max READ maximum WRITE setMaximum )
    Q_PROPERTY( double Value READ value WRITE setValue )
    Q_PROPERTY( double SingleStep READ singleStep WRITE setSingleStep )

  public:
    PowerSlider( QWidget* parent = 0, double alignmentValue = 100.0 );
    ~PowerSlider();
#ifdef ENABLE_QDOM
    QDomElement domElement( const QString& name, QDomDocument& document ) const;
    void initFromDOMElement( const QDomElement& element );
#endif
    double value();
    double minimum();
    double maximum();
    double singleStep();
  public slots:
    void setSingleStep( double );
    void setRange( double, double );
    void setMinimum( double );
    void setMaximum( double );

    void setValue( double );
  signals:
    void valueChanged( double );

  private:
    QSlider* slider_;
    QDoubleSpinBox* spinBox_;
  private slots:
    void on_slider_valueChanged( int );
    void on_spinBox_valueChanged( double );
};

} // namespace Ra::Gui::Widgets
