#include <Dataflow/QtGui/GraphEditor/WidgetFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {

void WidgetFactory::operator()( const std::string& name, std::string& p ) {
    auto frame  = new QFrame();
    auto layout = new QHBoxLayout();
    frame->setLayout( layout );
    auto line  = new QLineEdit( frame );
    auto label = new QLabel( QString::fromStdString( name ), frame );
    line->setObjectName( QString::fromStdString( name ) );
    QLineEdit::connect(
        line, &QLineEdit::textEdited, [&p]( const QString& string ) { p = string.toStdString(); } );
    layout->addWidget( label );
    layout->addWidget( line );
    variable_set_editor()->addWidget( frame );
}

#if HAS_TRANSFER_FUNCTION
/*
 * Transfer function
 */
WidgetFactory::registerWidget<std::array<float, 256 * 4>>(
    []( EditableParameterBase* editableParameter ) {
        auto editableTransferFunction =
            dynamic_cast<EditableParameter<std::array<float, 256 * 4>>*>( editableParameter );
        // TODO, give a name to the widget so that they can be updated automatically when
        // loading a node
        auto button         = new QPushButton( "Open widget" );
        auto transferEditor = new TransferEditor();
        TransferEditor::connect(
            transferEditor,
            &TransferEditor::propertiesChanged,
            [editableTransferFunction, transferEditor]() {
                int pos = 0;
                for ( int i = 0; i < 256; i++ ) {
                    unsigned int color = transferEditor->colorAt( i );
                    editableTransferFunction->m_data.at( pos ) =
                        (unsigned char)( ( 0x00ff0000 & color ) >> 16 ) / 255.f;
                    editableTransferFunction->m_data.at( pos + 1 ) =
                        (unsigned char)( ( 0x0000ff00 & color ) >> 8 ) / 255.f;
                    editableTransferFunction->m_data.at( pos + 2 ) =
                        (unsigned char)( ( 0x000000ff & color ) ) / 255.f;
                    editableTransferFunction->m_data.at( pos + 3 ) =
                        (unsigned char)( ( 0xff000000 & color ) >> 24 ) / 255.f;
                    pos = pos + 4;
                }
            } );
        QPushButton::connect(
            button, &QPushButton::clicked, [transferEditor]() { transferEditor->show(); } );
        return button;
    },
    []( QWidget*, EditableParameterBase* ) -> bool { return false; } );
#endif

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
