#include <Core/CoreMacros.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/ReduceNode.hpp>
#include <Dataflow/Core/Functionals/TransformNode.hpp>
#include <Dataflow/Core/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Sources/FunctionSource.hpp>
#include <Dataflow/Core/Sources/SingleDataSourceNode.hpp>

#include <iostream>
#include <limits>
#include <memory>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <vector>

using namespace Ra::Dataflow::Core;

/* ----------------------------------------------------------------------------------- */
/**
 * \brief Demonstrate how to use a graph to filter a collection
 */
int main( int argc, char* argv[] ) {
    DataflowGraph g { "helloGraph" };

    //! [Adding Nodes to the graph]
    using TransformNode           = Functionals::TransformNode<std::vector<Scalar>>;
    using TransformOperatorSource = Sources::FunctionSourceNode<Scalar, const Scalar&>;

    auto sourceNode    = g.add_node<Sources::SingleDataSourceNode<std::vector<Scalar>>>( "Source" );
    auto mapSource     = g.add_node<TransformOperatorSource>( "MapOperator" );
    auto transformNode = g.add_node<TransformNode>( "Transformer" );
    auto reduceNode    = g.add_node<Functionals::ReduceNode<std::vector<Scalar>>>( "Minimum" );
    auto sinkNode      = g.add_node<Sinks::SinkNode<std::vector<Scalar>>>( "Sink" );
    auto scalarSinkNode = g.add_node<Sinks::SinkNode<Scalar>>( "ScalarSink" );

    TransformNode::TransformOperator oneMinusMe = []( const Scalar& i ) -> Scalar {
        return 1_ra - i;
    };
    transformNode->setOperator( oneMinusMe );

    Functionals::ReduceNode<std::vector<Scalar>>::ReduceOperator getMin =
        []( const Scalar& a, const Scalar& b ) -> Scalar { return std::min( a, b ); };
    reduceNode->setOperator( getMin, std::numeric_limits<Scalar>::max() );

    TransformOperatorSource::function_type doubleMe = []( const Scalar& i ) -> Scalar {
        return 2_ra * i;
    };
    mapSource->set_data( doubleMe );

    //! [Adding Nodes to the graph]

    //! [Creating links between Nodes]
    g.add_link( sourceNode, "to", transformNode, "data" );
    g.add_link( transformNode, "result", sinkNode, "from" );
    g.add_link( transformNode, "result", reduceNode, "data" );
    g.add_link( reduceNode, "result", scalarSinkNode, "from" );
    //! [Creating links between Nodes]

    //! [Verifing the graph can be compiled]
    if ( !g.compile() ) {
        std::cout << " compilation failed";
        return 1;
    }
    //! [Verifing the graph can be compiled]

    //! [Initializing input variable to test the graph]
    std::vector<Scalar> test;

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
    sourceNode->set_data( test );
    //! [Initializing input variable to test the graph]

    //! [Execute the graph]
    g.execute();
    // The reference to the result is now available
    auto& result   = sinkNode->dataByRef();
    auto& minValue = scalarSinkNode->dataByRef();
    //! [Execute the graph]

    //! [Print the output result]
    std::cout << "Output values (oneMinusMe): " << result.size() << "\n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    std::cout << "Min value is : " << minValue << "\n";
    //! [Print the output result]

    //! [Verify the result]
    std::cout << "Verifying the result : \n\t";

    bool execOK { true };
    for ( int i = 0; i < 10; ++i ) {
        if ( result[i] != oneMinusMe( test[i] ) ) {
            std::cout << "Error at index " << i << " : " << result[i] << " should be "
                      << oneMinusMe( test[i] ) << "\n\t";
            execOK = false;
        }
    }
    if ( execOK ) { std::cout << "OK\n"; }
    else {
        std::cout << "NOK\n";
        return 1;
    }
    //! [Verify the result]

    //! [Modifying the graph to add link to map operator]
    if ( !g.add_link( mapSource, "to", transformNode, "op" ) ) {
        std::cout << "Unable to link port f ("
                  << ") from " << mapSource->instance_name() << " to port f("
                  << ") of " << transformNode->instance_name() << "\n";
        return 1;
    }
    //! [Modifying the graph to add link to map operator]

    //! [Execute and test the result]
    std::cout << "Rerun the graph with different operator : \n";
    g.execute();
    std::cout << "Output values (doubleMe): " << result.size() << "\n\t";
    for ( auto ord : result ) {
        std::cout << ord << ' ';
    }
    std::cout << '\n';
    std::cout << "Min value is : " << minValue << "\n";
    execOK = true;
    for ( int i = 0; i < 10; ++i ) {
        if ( result[i] != doubleMe( test[i] ) ) {
            std::cout << "Error at index " << i << " : " << result[i] << " should be "
                      << doubleMe( test[i] ) << "\n\t";
            execOK = false;
        }
    }
    if ( execOK ) { std::cout << "OK\n"; }
    else {
        std::cout << "NOK\n";
        return 1;
    }
    //! [Execute and test the result]

    return 0;
}
