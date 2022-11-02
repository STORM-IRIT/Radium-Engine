#include <Dataflow/QtGui/GraphEditor/WidgetFactory.hpp>

#include <Dataflow/QtGui/GraphEditor/GraphEditorView.hpp>

#include <Core/Utils/Color.hpp>
#include <Gui/Widgets/ControlPanel.hpp>

// #include <RadiumAddons/Gui/Widgets/TransferEditor.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Enumerator.hpp>
// #include <Dataflow/Core/Nodes/Color/ColorTransformNode.hpp>
// #include <Dataflow/Core/Nodes/RegisterStandardNodes.hpp>

#include <Engine/Data/EnvironmentTexture.hpp>
#include <PowerSlider/PowerSlider.hpp>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {

using namespace Ra::Dataflow::Core;
using namespace Ra::Gui::Widgets;

using namespace Ra::Engine::Data;

namespace WidgetFactory {
using WidgetFunctionPair = std::pair<WidgetCreatorFunc, WidgetUpdaterFunc>;

std::unordered_map<size_t, WidgetFunctionPair> widgetsfunctions;

void registerWidgetInternal( size_t hashedType,
                             WidgetCreatorFunc widgetCreator,
                             WidgetUpdaterFunc widgetUpdater ) {
    if ( widgetsfunctions.find( hashedType ) == widgetsfunctions.end() ) {
        widgetsfunctions[hashedType] = { std::move( widgetCreator ), std::move( widgetUpdater ) };
    }
    else {
        std::cerr
            << "WidgetFactory: trying to add an already existing widget builder for hashed type "
            << hashedType << "." << std::endl;
    }
}

QWidget* createWidget( EditableParameterBase* editableParameter ) {
    if ( widgetsfunctions.find( editableParameter->m_hashedType ) != widgetsfunctions.end() ) {
        return widgetsfunctions[editableParameter->m_hashedType].first( editableParameter );
    }
    else {
        std::cerr << "WidgetFactory: no defined widget builder for hashed type "
                  << editableParameter->m_hashedType << "." << std::endl;
    }
    return nullptr;
}

bool updateWidget( QWidget* widget, EditableParameterBase* editableParameter ) {
    if ( widgetsfunctions.find( editableParameter->m_hashedType ) != widgetsfunctions.end() ) {
        return widgetsfunctions[editableParameter->m_hashedType].second( widget,
                                                                         editableParameter );
    }
    else {
        std::cerr << "WidgetFactory: no defined widget updater for hashed type "
                  << editableParameter->m_hashedType << "." << std::endl;
    }
    return false;
}

void initializeWidgetFactory() {
    /*
     * Environment map "edition" widget
     */
    WidgetFactory::registerWidget<std::shared_ptr<EnvironmentTexture>>(
        []( EditableParameterBase* editableParameter ) {
            auto editable = dynamic_cast<EditableParameter<std::shared_ptr<EnvironmentTexture>>*>(
                editableParameter );

            auto controlPanel = new ControlPanel( editable->m_name, false );
            auto envmpClbck   = [editable, controlPanel]( const std::string& files ) {
                if ( files.empty() ) { editable->m_data = nullptr; }
                else {
                    // for now, only skyboxes are managed
                    editable->m_data = std::make_shared<EnvironmentTexture>( files, true );
                    auto slider      = controlPanel->findChild<PowerSlider*>( "strength" );
                    if ( slider ) { editable->m_data->setStrength( slider->value() / 100. ); }
                }
            };
            // TODO : display the name of the envmap image somewhere
            controlPanel->addFileInput(
                "files", envmpClbck, "../", "Images (*.png *.jpg *.pfm *.exr *hdr)" );

            auto strengthClbk = [editable]( double v ) {
                if ( editable->m_data ) {
                    auto* env = editable->m_data.get();
                    if ( env ) { env->setStrength( v / 100. ); }
                }
            };
            float s_init = 100.;
            if ( editable->m_data ) { s_init = editable->m_data->getStrength() * 100.; }
            controlPanel->addPowerSliderInput( "strength", strengthClbk, s_init, 0., 100 );
            controlPanel->setVisible( true );
            return controlPanel;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable = dynamic_cast<EditableParameter<std::shared_ptr<EnvironmentTexture>>*>(
                editableParameter );
            auto slider = widget->findChild<PowerSlider*>( "strength" );
            if ( slider && editable->m_data ) {
                slider->setValue( editable->m_data->getStrength() * 100. );
                return true;
            }
            else {
                return slider != nullptr;
            }
        } );

    /*
     * Scalar edition
     */
    WidgetFactory::registerWidget<Scalar>(
        []( EditableParameterBase* editableParameter ) {
            auto editable    = dynamic_cast<EditableParameter<Scalar>*>( editableParameter );
            auto powerSlider = new PowerSlider();
            powerSlider->setObjectName( editable->m_name.c_str() );
            editable->m_data = 0.0;
            powerSlider->setValue( editable->m_data );
            if ( editable->additionalData.size() >= 2 ) {
                powerSlider->setRange( editable->additionalData[0], editable->additionalData[1] );
            }
            else {
                powerSlider->setRange( 0.0, 9999.0 );
            }
            PowerSlider::connect( powerSlider,
                                  &PowerSlider::valueChanged,
                                  [editable]( Scalar value ) { editable->m_data = value; } );
            return powerSlider;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable = dynamic_cast<EditableParameter<Scalar>*>( editableParameter );
            auto slider   = widget->findChild<PowerSlider*>( editableParameter->m_name.c_str() );
            if ( slider ) {
                slider->setValue( editable->m_data );
                return true;
            }
            else {
                return false;
            }
        } );

    /*
     * Boolean edition
     */
    WidgetFactory::registerWidget<bool>(
        []( EditableParameterBase* editableParameter ) {
            auto editable = dynamic_cast<EditableParameter<bool>*>( editableParameter );
            auto checkBox = new QCheckBox();
            checkBox->setObjectName( editable->m_name.c_str() );
            checkBox->setCheckState( editable->m_data ? Qt::CheckState::Checked
                                                      : Qt::CheckState::Unchecked );
            QCheckBox::connect( checkBox, &QCheckBox::stateChanged, [editable]( int state ) {
                if ( state == Qt::Unchecked ) { editable->m_data = false; }
                else if ( state == Qt::Checked ) {
                    editable->m_data = true;
                }
            } );
            return checkBox;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable = dynamic_cast<EditableParameter<bool>*>( editableParameter );
            auto checkBox = widget->findChild<QCheckBox*>( editableParameter->m_name.c_str() );
            if ( checkBox ) {
                checkBox->setCheckState( editable->m_data ? Qt::Checked : Qt::Unchecked );
                return true;
            }
            else {
                return false;
            }
        } );

    /*
     * Color edition
     */
    WidgetFactory::registerWidget<Ra::Core::Utils::Color>(
        []( EditableParameterBase* editableParameter ) {
            auto editable =
                dynamic_cast<EditableParameter<Ra::Core::Utils::Color>*>( editableParameter );
            auto controlPanel = new ControlPanel( editable->m_name, false );
            auto clrCbk       = [editable]( const Ra::Core::Utils::Color& clr ) {
                editable->m_data = clr;
            };
            controlPanel->addColorInput( "Choose color", clrCbk, editable->m_data, true );
            controlPanel->setVisible( true );
            return controlPanel;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable =
                dynamic_cast<EditableParameter<Ra::Core::Utils::Color>*>( editableParameter );
            auto button = widget->findChild<QPushButton*>( "Choose color" );
            if ( button ) {
                // todo, update the color on the button and make the dialog to take its
                //  initial color from the button. Once dont, return true ...
                return false;
            }
            return false;
        } );

    /*
     * Button edition
     */
    WidgetFactory::registerWidget<DataflowGraph>(
        []( EditableParameterBase* editableParameter ) {
            auto editable = dynamic_cast<EditableParameter<DataflowGraph>*>( editableParameter );
            auto button   = new QPushButton( "Show graph" );
            button->setObjectName( editableParameter->m_name.c_str() );
            QPushButton::connect( button, &QPushButton::clicked, [editable]() {
                // Display a window to see the graph
                /*auto graph = reinterpret_cast<DataflowGraph*>( editable );
                auto nodeEditor          = new GraphEditorView( nullptr );

                nodeEditor->editGraph( nullptr );
                nodeEditor->editGraph( graph );

                nodeEditor->resize( 900, 500 );
                nodeEditor->move( 450, 260 );
                nodeEditor->show(); */
            } );

            return button;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable = dynamic_cast<EditableParameter<DataflowGraph>*>( editableParameter );
            auto button   = widget->findChild<QPushButton*>( editable->m_name.c_str() );
            if ( button ) { return true; }
            else {
                return false;
            }
        } );

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
    /*
     * String enumerator
     */
    WidgetFactory::registerWidget<Enumerator<std::string>>(
        []( EditableParameterBase* editableParameter ) {
            auto editable =
                dynamic_cast<EditableParameter<Enumerator<std::string>>*>( editableParameter );
            auto selector = new QComboBox();
            selector->setObjectName( editable->m_name.c_str() );
            for ( const auto& e : editable->m_data ) {
                selector->addItem( e.c_str() );
            }
            QComboBox::connect(
                selector, &QComboBox::currentTextChanged, [editable]( const QString& string ) {
                    editable->m_data.set( string.toStdString() );
                } );
            return selector;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable =
                dynamic_cast<EditableParameter<Enumerator<std::string>>*>( editableParameter );
            auto comboBox = widget->findChild<QComboBox*>( editable->m_name.c_str() );
            if ( comboBox ) {
                comboBox->setCurrentText( editable->m_data.get().c_str() );
                return true;
            }
            else {
                return false;
            }
        } );

    /*
     * String
     */
    WidgetFactory::registerWidget<std::string>(
        []( EditableParameterBase* editableParameter ) {
            auto editable = dynamic_cast<EditableParameter<std::string>*>( editableParameter );
            auto line     = new QLineEdit();
            line->setObjectName( editable->m_name.c_str() );
            QLineEdit::connect( line, &QLineEdit::textEdited, [editable]( const QString& string ) {
                editable->m_data = string.toStdString();
            } );
            return line;
        },
        []( QWidget* widget, EditableParameterBase* editableParameter ) -> bool {
            auto editable = dynamic_cast<EditableParameter<std::string>*>( editableParameter );
            auto line     = widget->findChild<QLineEdit*>( editable->m_name.c_str() );
            if ( line ) {
                line->setText( editable->m_data.c_str() );
                return true;
            }
            else {
                return false;
            }
        } );
    /*
     * Ra::Engine::Data::ShaderConfiguration --> Code Editor
     */
}

} // namespace WidgetFactory

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
