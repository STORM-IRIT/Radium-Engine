#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Filters/FilterNode.hpp>
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

using namespace Ra::Dataflow::Core;

/* ----------------------------------------------------------------------------------- */
/**
 * \brief Demonstrate how to use a graph to filter a collection
 */
int main( int argc, char* argv[] ) {
    //! [Creating an empty graph]
    DataflowGraph g { "helloGraph" };
    //! [Creating an empty graph]

    //! [Creating Nodes]
    auto sourceNode = new Sources::SingleDataSourceNode<std::vector<Scalar>>( "Source" );
    // non serializable node using a custom filter
    auto filterNode =
        new Filters::FilterNode<Scalar>( "Filter", []( Scalar x ) { return x > 0.5_ra; } );
    auto sinkNode = new Sinks::SinkNode<std::vector<Scalar>>( "Sink" );
    //! [Creating Nodes]

    //! [Adding Nodes to the graph]
    g.addNode( sourceNode );
    g.addNode( filterNode );
    g.addNode( sinkNode );
    //! [Adding Nodes to the graph]

    //! [Creating links between Nodes]
    g.addLink( sourceNode, "to", filterNode, "in" );
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

    //! [Verifing the graph can be compiled]
    if ( !g.compile() ) {
        std::cout << " compilation failed";
        return 1;
    }
    //! [Verifing the graph can be compiled]

    ///! [Configure the interface ports (input and output of the graph)
    auto input = g.getDataSetter( "Source_to" );
    std::vector<Scalar> test;
    input->setData( &test );
    auto output = g.getDataGetter( "Sink_from" );
    // The reference to the result will not be available before the first run
    // auto& result = output->getData<std::vector<Scalar>>();
    ///! [Configure the interface ports (input and output of the graph)

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
    // The reference to the result is now available
    auto& result = output->getData<std::vector<Scalar>>();
    //! [Execute the graph]

    //! [Print the output result]
    std::cout << "Output values (reference): \n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [Print the output result]

    //! [Modify input and rerun the graph]
    // here, we of re-use the same interface objects (pointer for input, reference for output)
    for ( int n = 0; n < 10; ++n ) {
        test.push_back( dis( gen ) );
    }
    g.execute();
    std::cout << "Output values after second execution: \n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    //! [Modify input and rerun the graph]

    return 0;
}
