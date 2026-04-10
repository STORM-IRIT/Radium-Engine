#include "Dataflow/RaDataflow.hpp"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <Dataflow/Core/Sources/Types.hpp>
#include <Dataflow/QtGui/GraphEditor/GraphEditorWindow.hpp>

#include <QtNodes/ConnectionStyle>

static void setStyle() {
    QtNodes::ConnectionStyle::setConnectionStyle(
        R"( { "ConnectionStyle": { "UseDataDefinedColors": true } } )" );
}

class StringInput : public Ra::Dataflow::Core::Sources::SingleDataSourceNode<std::string>
{
  public:
    using base = Ra::Dataflow::Core::Sources::SingleDataSourceNode<std::string>;
    explicit StringInput( const std::string& name ) : base( name, node_typename() ) {}
    RA_NODE_TYPENAME( "StringInput" );
};

class SquareFunction : public Ra::Dataflow::Core::Sources::FunctionSourceNode<Scalar, const Scalar&>
{
  public:
    using base = Ra::Dataflow::Core::Sources::FunctionSourceNode<Scalar, const Scalar&>;
    explicit SquareFunction( const std::string& name ) : base( name, node_typename() ) {
        set_data( []( const Scalar& b ) { return b * b; } );
    }
    RA_NODE_TYPENAME( "SquareFunction" );
};

int main( int argc, char* argv[] ) {
    QApplication app( argc, argv );

    auto port_fatcory = Ra::Dataflow::Core::PortFactory::getInstance();

    setStyle();

    auto coreFactory = Ra::Dataflow::Core::NodeFactoriesManager::default_factory();
    // add node creators to the factory
    REGISTER_TYPE_TO_FACTORY( coreFactory, SquareFunction, "Sources" );
    REGISTER_TYPE_TO_FACTORY( coreFactory, StringInput, "Sources" );

    QCoreApplication::setOrganizationName( "STORM-IRIT" );
    QCoreApplication::setApplicationName( "Radium NodeGraph Editor" );
    QCoreApplication::setApplicationVersion( QT_VERSION_STR );
    QCommandLineParser parser;
    parser.setApplicationDescription( QCoreApplication::applicationName() );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "file", "The file to open." );
    parser.process( app );

    Ra::Dataflow::QtGui::GraphEditor::GraphEditorWindow mainWin;
    if ( !parser.positionalArguments().isEmpty() )
        mainWin.loadFile( parser.positionalArguments().first() );
    mainWin.show();
    return app.exec();
}
