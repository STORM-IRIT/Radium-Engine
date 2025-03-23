#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>
#include <Dataflow/QtGui/GraphEditor/GraphEditorWindow.hpp>

#include <QtNodes/ConnectionStyle>

static void setStyle() {
    QtNodes::ConnectionStyle::setConnectionStyle(
        R"( { "ConnectionStyle": { "UseDataDefinedColors": true } } )" );
}

class SquareFunction : public Ra::Dataflow::Core::Sources::FunctionSourceNode<Scalar, const Scalar&>
{
  public:
    using base = Ra::Dataflow::Core::Sources::FunctionSourceNode<Scalar, const Scalar&>;
    SquareFunction( const std::string& name ) : base( name, getTypename() ) {
        setData( []( const Scalar& b ) { return b * b; } );
    }
    static const std::string& getTypename() {
        static std::string n { "SquareFunction" };
        return n;
    }
};

int main( int argc, char* argv[] ) {
    QApplication app( argc, argv );

    auto port_fatcory = Ra::Dataflow::Core::PortFactory::createInstance();
    port_fatcory->add_port_type<Scalar>();

    setStyle();

    auto coreFactory = Ra::Dataflow::Core::NodeFactoriesManager::getDataFlowBuiltInsFactory();
    // add node creators to the factory
    coreFactory->registerNodeCreator<SquareFunction>( SquareFunction::getTypename(), "Sources" );

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
