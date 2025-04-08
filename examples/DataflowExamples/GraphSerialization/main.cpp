#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Sources/SingleDataSourceNode.hpp>

#include <Core/Containers/VectorArray.hpp>

#include <random>

using namespace Ra::Dataflow::Core;

/* ----------------------------------------------------------------------------------- */
/**
 * \brief Demonstrate how serialize a graph with custom nodes.
 */
int main( int argc, char* argv[] ) {

    //! [Creating the factory for the custom nodes and add it to the nodes system]
    using VectorType = std::vector<Scalar>;
    // using VectorType = Ra::Core::VectorArray<Scalar>;
    // custom node type are either specialization of templated nodes or user-define nodes class

    // create the custom node factory
    auto customFactory = NodeFactoriesManager::createFactory( "ExampleCustomFactory" );
    // add node creators to the factory
    customFactory->registerNodeCreator<Sources::SingleDataSourceNode<VectorType>>(
        Sources::SingleDataSourceNode<VectorType>::getTypename() + "_", "Custom" );
    customFactory->registerNodeCreator<Functionals::FilterNode<VectorType>>(
        Functionals::FilterNode<VectorType>::getTypename() + "_", "Custom" );
    customFactory->registerNodeCreator<Sinks::SinkNode<VectorType>>(
        Sinks::SinkNode<VectorType>::getTypename() + "_", "Custom" );

    //! [Creating the factory for the custom node types and add it to the node system]

    //! [Creating input variable to test the graph]
    VectorType test { 0.592845,
                      0.844266,
                      0.857946,
                      0.847252,
                      0.623564,
                      0.384382,
                      0.297535,
                      0.056713,
                      0.272656,
                      0.477665 };

    std::cout << "Input values : \n\t";
    for ( auto v : test ) {
        std::cout << v << ' ';
    }
    std::cout << '\n';
    //! [Creating input variable to test the graph]

    {
        //! [Creating an empty graph using the custom nodes factory]
        DataflowGraph g { "Serialization example" };
        //! [Creating an empty graph using the custom nodes factory]

        //! [Creating Nodes]
        auto sourceNode = g.addNode<Sources::SingleDataSourceNode<VectorType>>( "Source" );
        // non serializable node using a custom filter
        auto filterNode = g.addNode<Functionals::FilterNode<VectorType>>(
            "Filter", []( const Scalar& x ) { return x > 0.5_ra; } );
        auto storeNode = g.addNode<Sinks::SinkNode<VectorType>>( "Store" );
        //! [Creating Nodes]

        //! [Creating links between Nodes]
        g.addLink( sourceNode, "to", filterNode, "data" );
        g.addLink( filterNode, "result", storeNode, "from" );
        //! [Creating links between Nodes]

        //! [Verifing the graph can be compiled]
        if ( !g.compile() ) {
            std::cout << " compilation failed";
            return 1;
        }
        //! [Verifing the graph can be compiled]

        sourceNode->setData( test );
        std::cout << "Executing the initial graph ...\n";
        g.execute();
        VectorType result = storeNode->getData();

        std::cout << "Output values : \n\t";
        for ( auto v : result ) {
            std::cout << v << ' ';
        }
        std::cout << '\n';

        //! [Serializing the graph]
        g.saveToJson( "GraphSerializeExample.json" );
        //! [Serializing the graph]
    }

    std::cout << "Loading and using the graph\n";

    //! [Creating an empty graph and load it from a file]
    DataflowGraph g1 { "" };
    if ( !g1.loadFromJson( "GraphSerializeExample.json" ) ) {
        std::cerr << "Could not load the graph\n";
        return 2;
    }
    //! [Creating an empty graph and load it from a file]

    //! [Verifing the graph can be compiled]
    if ( !g1.compile() ) {
        std::cerr << "Compilation failed for the loaded graph";
        return 3;
    }
    //! [Verifing the graph can be compiled]

    //! [Execute the graph]
    std::cout << "Executing the loaded graph ...\n";
    g1.execute();
    //! [Execute the graph]

    //! [Print the output result]
    auto output = g1.getDataGetter( "Store", "data" );

    VectorType result = output->getData<VectorType>();

    std::cout << "Output values : \n\t";
    for ( auto v : result ) {
        std::cout << v << ' ';
    }
    std::cout << '\n';
    //! [Print the output result]

    //! [Set the correct filter on the filter node]
    auto filter =
        std::dynamic_pointer_cast<Functionals::FilterNode<VectorType>>( g1.getNode( "Filter" ) );
    if ( !filter ) {
        std::cerr << "Unable to cast the filter to "
                  << Ra::Core::Utils::simplifiedDemangledType( filter ) << "\n";
        return 4;
    }
    filter->set_predicate( []( const Scalar& f ) { return f > 0.5_ra; } );
    //! [Set the correct filter on the filter node]

    //! [Execute the graph]
    std::cout << "Executing the re-parameterized graph ...\n";
    g1.execute();
    result = output->getData<VectorType>();
    std::cout << "Output values : \n\t";
    for ( auto v : result ) {
        std::cout << v << ' ';
    }
    std::cout << '\n';
    //! [Execute the graph]

    return 0;
}
