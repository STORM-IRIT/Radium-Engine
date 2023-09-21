#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

using namespace Ra::Dataflow::Core;

/* ----------------------------------------------------------------------------------- */
/**
 * \brief Demonstrate how to use a graph to filter a collection
 */
int main( int argc, char* argv[] ) {
    using RaVector = Ra::Core::VectorArray<Scalar>;
    //! [Creating an empty graph]
    DataflowGraph g { "helloGraph" };
    //! [Creating an empty graph]

    //! [Creating Nodes]
    auto sourceNode    = g.addNode<Sources::SingleDataSourceNode<RaVector>>( "Source" );
    auto predicateNode = g.addNode<Sources::ScalarUnaryPredicateSource>( "Selector" );
    auto filterNode    = g.addNode<Functionals::FilterNode<RaVector>>( "Filter" );
    auto sinkNode      = g.addNode<Sinks::SinkNode<RaVector>>( "Sink" );
    //! [Creating Nodes]

    //! [Creating links between Nodes]
    g.addLink( sourceNode, sourceNode->getOuputPort(), filterNode, filterNode->getInPort() );
    if ( !g.addLink( predicateNode, "f", filterNode, "f" ) ) {
        std::cerr << "Error, can't link functional ports !\n";
        std::abort();
    }
    g.addLink( filterNode, "out", sinkNode, "from" );
    //! [Creating links between Nodes]

    //! [Inspect the graph interface : inputs and outputs port]
    auto inputs = g.getAllDataSetters();
    std::cout << "Input ports (" << inputs.size() << ") are :\n";
    for ( auto& [ptrPort, portName, portType] : inputs ) {
        std::cout << "\t\"" << portName << "\" accepting type " << portType << "\n";
    }
    auto outputs = g.getAllDataGetters();
    std::cout << "Output ports (" << outputs.size() << ") are :\n";
    for ( auto& [ptrPort, portName, portType] : outputs ) {
        std::cout << "\t\"" << portName << "\" generating type " << portType << "\n";
    }
    //! [Inspect the graph interface : inputs and outputs port]

    //! [Verifying the graph can be compiled]
    if ( !g.compile() ) {
        std::cout << " compilation failed";
        return 1;
    }
    //! [Verifying the graph can be compiled]

    g.saveToJson( "illustrateDoc.json" );

    ///! [Configure the interface ports (input and output of the graph)]
    auto input = g.getDataSetter( "Source_to" );
    RaVector test;
    input->setData( &test );

    auto selector                                           = g.getDataSetter( "Selector_f" );
    Sources::ScalarUnaryPredicateSource::function_type pred = []( Scalar x ) { return x < 0.5; };
    selector->setData( &pred );

    auto output = g.getDataGetter( "Sink_from" );
    // The reference to the result will not be available before the first run
    // auto& result = output->getData<std::vector<Scalar>>();
    ///! [Configure the interface ports (input and output of the graph)]

    //! [Initializing input variable to test the graph]
    test.reserve( 10 );
    std::mt19937 gen( 0 );
    std::uniform_real_distribution<> dis( 0.0, 1.0 );
    // Fill the vector with random numbers between 0 and 1
    for ( int n = 0; n < test.capacity(); ++n ) {
        test.push_back( dis( gen ) );
    }

    std::cout << "Input values : \n\t";
    for ( auto ord : test ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [Initializing input variable to test the graph]

    //! [Execute the graph]
    g.execute();
    // The reference to the result is now available. Results can be accessed through a reference
    // (sort of RVO from the graph https://en.cppreference.com/w/cpp/language/copy_elision)
    // or copied in an application variable.
    auto& result = output->getData<RaVector>();
    //! [Execute the graph]

    //! [Print the output result]
    std::cout << "Output values (reference): " << result.size() << "\n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [Print the output result]

    //! [Modify input and rerun the graph]
    Sources::ScalarUnaryPredicateSource::function_type predbig = []( Scalar x ) { return x > 0.5; };
    selector->setData( &predbig );
    g.execute();
    std::cout << "Output values after second execution: " << result.size() << "\n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [Modify input and rerun the graph]

    //! [Disconnect data setter and rerun the graph - result is now empty]
    g.releaseDataSetter( "Source_to" );
    g.execute();
    std::cout << "Output values after third execution: " << result.size() << "\n";
    //! [Disconnect data setter and rerun the graph - result is now empty]

    //! [As interface is disconnected, we can set data direclty on the source node]
    sourceNode->setData( std::move( test ) );
    g.execute();
    std::cout << "Output values after fourth execution: " << result.size() << "\n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [As interface is disconnected, we can set data direclty on the source node]

    //! [Reconnect data setter and rerun the graph - result is the same than second execution]
    g.activateDataSetter( "Source_to" );
    g.execute();
    std::cout << "Output values after last execution: " << result.size() << "\n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [Reconnect data setter and rerun the graph - result is the same than second execution]
    return 0;
}
