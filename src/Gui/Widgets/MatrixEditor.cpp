#include <Gui/Widgets/MatrixEditor.hpp>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QValidator>
#include <QWidget>

#include <memory>

namespace Ra::Gui::Widgets {

MatrixEditor::MatrixEditor( const Ra::Core::MatrixN& matrix, int dec, QWidget* parent ) :
    QWidget( parent ), m_matrix( matrix ) {

    QGridLayout* layout = new QGridLayout;
    layout->setMargin( 0 );
    layout->setSpacing( 0 );

    for ( long i = 0; i < m_matrix.rows(); ++i ) {
        for ( int j = 0; j < m_matrix.cols(); ++j ) {
            auto& elem   = m_matrix( i, j );
            auto spinbox = new QDoubleSpinBox();
            spinbox->setDecimals( dec );
            spinbox->setValue( elem );
            layout->addWidget( spinbox, i, j );
            auto updateMatrixOnChange = [this, &elem]( double value ) {
                elem = value;
                emit valueChanged( m_matrix );
            };
            connect( spinbox,
                     QOverload<double>::of( &QDoubleSpinBox::valueChanged ),
                     updateMatrixOnChange );
        }
    }

    setLayout( layout );
}

const Ra::Core::MatrixN& MatrixEditor::matrix() const {
    return m_matrix;
}
} // namespace Ra::Gui::Widgets
