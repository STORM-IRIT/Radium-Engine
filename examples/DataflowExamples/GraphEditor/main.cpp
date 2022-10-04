#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "MainWindow.hpp"

#include <Dataflow/Core/Nodes/Functionals/FilterNode.hpp>
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

    // [Creating the factory for the custom nodes and add it to the nodes system]
    using VectorType = std::vector<Scalar>;
    // using VectorType = Ra::Core::VectorArray<Scalar>;
    // custom node type are either specialization of templated nodes or user-define nodes class

    // create the custom node factory
    NodeFactorySet::mapped_type customFactory {
        new NodeFactorySet::mapped_type::element_type( "ExampleCustomFactory" ) };

    // add node creators to the factory
    customFactory->registerNodeCreator<Sources::SingleDataSourceNode<VectorType>>(
        Sources::SingleDataSourceNode<VectorType>::getTypename() + "_", "Custom" );
    customFactory->registerNodeCreator<Functionals::FilterNode<VectorType>>(
        Functionals::FilterNode<VectorType>::getTypename() + "_", "Custom" );
    customFactory->registerNodeCreator<Sinks::SinkNode<VectorType>>(
        Sinks::SinkNode<VectorType>::getTypename() + "_", "Custom" );

    // register the factory into the system to enable loading any graph that use these nodes
    NodeFactoriesManager::registerFactory( customFactory );
    // [Creating the factory for the custom node types and add it to the node system]

    MainWindow mainWin;
    if ( !parser.positionalArguments().isEmpty() )
        mainWin.loadFile( parser.positionalArguments().first() );
    mainWin.show();
    return app.exec();
}
