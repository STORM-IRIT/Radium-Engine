#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "MainWindow.hpp"

#include <Dataflow/Core/Nodes/Filters/FilterNode.hpp>
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

using namespace Ra::Dataflow::Core;

int main( int argc, char* argv[] ) {
    Q_INIT_RESOURCE( GraphEditor );

    QApplication app( argc, argv );
    QCoreApplication::setOrganizationName( "STORM-IRIT" );
    QCoreApplication::setApplicationName( "Radium flowgraph example" );
    QCoreApplication::setApplicationVersion( QT_VERSION_STR );
    QCommandLineParser parser;
    parser.setApplicationDescription( QCoreApplication::applicationName() );
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument( "file", "The file to open." );
    parser.process( app );

    // create the custom node factory
    NodeFactorySet::mapped_type customFactory {
        new NodeFactorySet::mapped_type::element_type( "ExampleCustomFactory" ) };

    // add node creators to the factory
    customFactory->registerNodeCreator<Sources::SingleDataSourceNode<std::vector<Scalar>>>(
        Sources::SingleDataSourceNode<std::vector<Scalar>>::getTypename() + "_", "Source" );
    customFactory->registerNodeCreator<Filters::FilterNode<Scalar>>(
        Filters::FilterNode<Scalar>::getTypename() + "_", "Filters" );
    customFactory->registerNodeCreator<Sinks::SinkNode<std::vector<Scalar>>>(
        Sinks::SinkNode<std::vector<Scalar>>::getTypename() + "_", "Sink" );

    // register the factory into the system to enable loading any graph that use these nodes
    NodeFactoriesManager::registerFactory( customFactory );

    MainWindow mainWin;
    if ( !parser.positionalArguments().isEmpty() )
        mainWin.loadFile( parser.positionalArguments().first() );
    mainWin.show();
    return app.exec();
}
