#include <catch2/catch.hpp>

#include <iostream>

#include <Core/Containers/MakeShared.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Functionals/BinaryOpNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/ReduceNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/TransformNode.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

using namespace Ra::Dataflow::Core;
using namespace Ra::Core;

TEST_CASE( "Dataflow/Core/GraphAsNode", "[Dataflow][Core][Graph]" ) {
    using RaVector = Ra::Core::VectorArray<Scalar>;

    auto port_fatcory = PortFactory::createInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = make_shared<DataflowGraph>( "graphAsNode" );

    // compute delta = b2 - 4ac;

    auto b2 = gAsNode->addNode<Functionals::FunctionNode<Scalar>>( "b2" );
    b2->setFunction( []( const Scalar& b ) { return b * b; } );

    auto fourAC = gAsNode->addNode<Functionals::BinaryOpNode<Scalar>>(
        "4ac", []( const Scalar& a, const Scalar& c ) { return 4_ra * a * c; } );

    auto b2minus4ac = gAsNode->addNode<Functionals::BinaryOpNode<Scalar>>(
        "b2-4ac", []( const Scalar& x, const Scalar& y ) { return x - y; } );

    auto forwardA = gAsNode->addNode<Functionals::FunctionNode<Scalar>>( "a" );
    auto forwardB = gAsNode->addNode<Functionals::FunctionNode<Scalar>>( "b" );
    auto forwardC = gAsNode->addNode<Functionals::FunctionNode<Scalar>>( "c" );

    b2minus4ac->getOutputPort()->setName( "delta" );

    REQUIRE( !gAsNode->input_node() );
    REQUIRE( !gAsNode->output_node() );

    gAsNode->add_input_output_nodes();
    auto inputA = gAsNode->input_node()->add_output_port( forwardA->getInPort().get() );
    auto inputB = gAsNode->input_node()->add_output_port( forwardB->getInPort().get() );
    auto inputC = gAsNode->input_node()->add_output_port( forwardC->getInPort().get() );
    auto output = gAsNode->output_node()->add_input_port( b2minus4ac->getOutputPort().get() );

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 3 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 1 );
    REQUIRE( gAsNode->getInputs().size() == 0 );
    REQUIRE( gAsNode->getOutputs().size() == 0 );

    gAsNode->input_node()->getInputByIndex( inputA )->setName( "a" );
    gAsNode->input_node()->getInputByIndex( inputB )->setName( "b" );
    gAsNode->input_node()->getInputByIndex( inputC )->setName( "c" );
    gAsNode->output_node()->getOutputByIndex( output )->setName( "delta" );

    gAsNode->addLink( forwardB->getOutPort(), b2->getInPort() );
    gAsNode->addLink( forwardA->getOutPort(), fourAC->getPortA() );
    gAsNode->addLink( forwardC->getOutPort(), fourAC->getPortB() );

    gAsNode->addLink( b2->getOutPort(), b2minus4ac->getPortA() );
    gAsNode->addLink( fourAC->getOutputPort(), b2minus4ac->getPortB() );

    REQUIRE( gAsNode->compile() );

    REQUIRE( gAsNode->getInputs().size() == 3 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );

    DataflowGraph g { "mainGraph" };
    auto sourceNodeA = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "sa" );
    auto sourceNodeB = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "sb" );
    auto sourceNodeC = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "sc" );

    auto resultNode = g.addNode<Sinks::SinkNode<Scalar>>( "odelta" );

    g.addNode( gAsNode );

    g.addLink( sourceNodeA->getOutPort().get(), gAsNode->getInputByIndex( inputA ) );
    g.addLink( sourceNodeB->getOutPort().get(), gAsNode->getInputByIndex( inputB ) );
    g.addLink( sourceNodeC->getOutPort().get(), gAsNode->getInputByIndex( inputC ) );
    g.addLink( gAsNode->getOutputByIndex( output ), resultNode->getInPort().get() );

    sourceNodeA->setData( 1 );
    sourceNodeB->setData( 2 );
    sourceNodeC->setData( 3 );

    auto p = resultNode->getInPort();
    REQUIRE( g.compile() );
    REQUIRE( g.execute() );

    auto& result = resultNode->getDataByRef();
    REQUIRE( result == -8 );

    ///\todo test addlink that creates ports

    ///\todo make own test with serializable nodes and checks.
    std::string tmpdir { "tmpDir4Tests" };
    std::filesystem::create_directories( tmpdir );
    g.saveToJson( tmpdir + "/graph_as_node.json" );
    // Create a new graph and load from the saved graph
    DataflowGraph g1 { "loaded graph" };
    g1.loadFromJson( tmpdir + "/graph_as_node.json" );
}
