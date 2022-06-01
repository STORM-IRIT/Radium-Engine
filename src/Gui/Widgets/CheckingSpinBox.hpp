#pragma once
#include <Gui/RaGui.hpp>

#include <QSpinBox>
#include <QWidget>

namespace Ra::Gui::Widgets {

/**
 * A DoubleSpinBox that check if its value match a condition defined by a predicate
 *
 */
class RA_GUI_API CheckingSpinBox : public QWidget
{
    Q_OBJECT

  public:
    /** Constructors
     */
    /** @{ */
    explicit CheckingSpinBox( QWidget* parent = nullptr );
    CheckingSpinBox( const CheckingSpinBox& )            = delete;
    CheckingSpinBox& operator=( const CheckingSpinBox& ) = delete;
    CheckingSpinBox( CheckingSpinBox&& )                 = delete;
    CheckingSpinBox&& operator=( CheckingSpinBox&& )     = delete;
    /**@}*/

    /**
     * @return the last valid value of the spin box
     */
    double value() const;

    /**
     * @return step value of the spin box
     */
    double singleStep() const;

    /**
     * Set the step value of the spin box, the default value is 1.0.
     * @param val the new value of the step
     * */
    void setSingleStep( double val );

    /**
     * Set the predicate function, the default returns true for every value
     * @param predicate the predicate function
     */
    void setPredicate( std::function<bool( double )> predicate );

    /**
     * @return the number of decimal of the spin box
     */
    int decimals() const;

    /**
     * Set the precision of the spin box, in decimals
     * @param prec the precision in decimals
     */
    void setDecimals( int prec );

  public slots:
    /**
     * Set the value of the spin box
     * @note the value should meet the predicate requierements
     * @param val the new value
     */
    void setValue( double val );

  private slots:
    void spinboxValueChanged( double val );

  signals:
    /**
     * signal emmited when the value of the spinbox changes
     */
    void valueChanged( double );

  private:
    std::function<bool( double )> m_predicate = []( double ) { return true; };
    QDoubleSpinBox* m_spinbox;
    double m_value;
};

} // namespace Ra::Gui::Widgets
