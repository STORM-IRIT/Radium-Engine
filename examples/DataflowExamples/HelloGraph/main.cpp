#include <Core/Containers/VectorArray.hpp>
#include <Core/CoreMacros.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Sources/FunctionSource.hpp>
#include <Dataflow/Core/Sources/SingleDataSourceNode.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

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
    auto sourceNode    = g.add_node<Sources::SingleDataSourceNode<RaVector>>( "Source" );
    auto predicateNode = g.add_node<Sources::ScalarUnaryPredicateSource>( "Selector" );
    auto filterNode    = g.add_node<Functionals::FilterNode<RaVector>>( "Filter" );
    auto sinkNode      = g.add_node<Sinks::SinkNode<RaVector>>( "Sink" );
    //! [Creating Nodes]

    //! [Creating links between Nodes]
    // link using nodes port, with compile time type check
    // node belongship to the graph is checked at runtime
    if ( !g.add_link( sourceNode->port_out_to(), filterNode->port_in_data() ) ) { std::abort(); }

    // link with port names, all runtime check
    if ( !g.add_link( predicateNode, "to", filterNode, "predicate" ) ) { std::abort(); }
    // one can also link using node index, it depends on node init, so be sure to have the right
    // index
    // Functional Port "out" as index 0, and Sink "from" is 0 also
    if ( !g.add_link( filterNode, Node::PortIndex { 0 }, sinkNode, Node::PortIndex { 0 } ) ) {
        std::abort();
    }
    //! [Creating links between Nodes]

    //! [Verifying the graph can be compiled]
    if ( !g.compile() ) {
        std::cout << " compilation failed";
        return 1;
    }
    //! [Verifying the graph can be compiled]

    //! [Configure nodes]
    RaVector test {
        0.0_ra, 1.0_ra, 0.1_ra, 0.9_ra, 0.2_ra, 0.8_ra, 0.3_ra, 0.7_ra, 0.4_ra, 0.6_ra, 0.5_ra };
    g.node<Sources::SingleDataSourceNode<RaVector>>( "Source" )->set_data( test );

    Sources::ScalarUnaryPredicateSource::function_type pred = []( Scalar x ) { return x < 0.5; };
    predicateNode->set_data( pred );
    //! [Configure nodes]

    std::cout << "Input values : \n\t";
    std::copy( test.begin(), test.end(), std::ostream_iterator<Scalar>( std::cout, " " ) );
    std::cout << '\n';

    //! [Execute the graph]
    if ( !g.execute() ) {
        std::cerr << "execution failed!\n";
        return 1;
    }
    //! [Execute the graph]

    //! [Print the output result]
    auto& result = sinkNode->data_reference();
    std::cout << "Output values: " << result.size() << "\n\t";
    std::copy( result.begin(), result.end(), std::ostream_iterator<Scalar>( std::cout, " " ) );
    std::cout << '\n';
    //! [Print the output result]

    //! [Modify input and rerun the graph]
    Sources::ScalarUnaryPredicateSource::function_type predbig = []( Scalar x ) { return x > 0.5; };
    predicateNode->set_data( predbig );

    g.execute();
    // since result is a ref to node's output, no need to get it again
    std::cout << "Output values after second execution: " << result.size() << "\n\t";
    std::copy( result.begin(), result.end(), std::ostream_iterator<Scalar>( std::cout, " " ) );
    std::cout << '\n';
    //! [Modify input and rerun the graph]

    return 0;
}
