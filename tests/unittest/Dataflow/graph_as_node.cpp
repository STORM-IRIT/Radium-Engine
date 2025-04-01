#include <catch2/catch_test_macros.hpp>

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

TEST_CASE( "Dataflow/Core/GraphAsNode", "[unittests][Dataflow][Core][Graph]" ) {
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

    REQUIRE( gAsNode->addLink( forwardB->getOutPort(), b2->getInPort() ) );
    REQUIRE( gAsNode->addLink( forwardA->getOutPort(), fourAC->getPortA() ) );
    REQUIRE( gAsNode->addLink( forwardC->getOutPort(), fourAC->getPortB() ) );

    REQUIRE( gAsNode->addLink( b2->getOutPort(), b2minus4ac->getPortA() ) );
    REQUIRE( gAsNode->addLink( fourAC->getOutputPort(), b2minus4ac->getPortB() ) );

    REQUIRE( gAsNode->compile() );

    REQUIRE( gAsNode->getInputs().size() == 3 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );

    DataflowGraph g { "mainGraph" };
    auto sourceNodeA = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "sa" );
    auto sourceNodeB = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "sb" );
    auto sourceNodeC = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "sc" );

    auto resultNode = g.addNode<Sinks::SinkNode<Scalar>>( "odelta" );

    REQUIRE( g.addNode( gAsNode ) );

    REQUIRE( g.addLink( sourceNodeA->getOutPort().get(), gAsNode->getInputByIndex( inputA ) ) );
    REQUIRE( g.addLink( sourceNodeB->getOutPort().get(), gAsNode->getInputByIndex( inputB ) ) );
    REQUIRE( g.addLink( sourceNodeC->getOutPort().get(), gAsNode->getInputByIndex( inputC ) ) );
    REQUIRE( g.addLink( gAsNode->getOutputByIndex( output ), resultNode->getInPort().get() ) );

    sourceNodeA->setData( 1 );
    sourceNodeB->setData( 2 );
    sourceNodeC->setData( 3 );

    REQUIRE( g.compile() );
    REQUIRE( g.execute() );

    auto& result = resultNode->getDataByRef();
    REQUIRE( result == -8 );
}

TEST_CASE( "Dataflow/Core/GraphAsNode/IO", "[unittests][Dataflow][Core][Graph]" ) {

    using PortIndex    = Ra::Dataflow::Core::Node::PortIndex;
    using FunctionNode = Functionals::FunctionNode<Scalar>;
    using Source       = Sources::SingleDataSourceNode<Scalar>;
    using Sink         = Sinks::SinkNode<Scalar>;

    auto port_fatcory = PortFactory::createInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = make_shared<DataflowGraph>( "graphAsNode" );

    // compute delta = b2 - 4ac;

    auto f = gAsNode->addNode<FunctionNode>( "f" );

    REQUIRE( !gAsNode->input_node() );
    REQUIRE( !gAsNode->output_node() );

    gAsNode->add_input_output_nodes();

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );

    // add link to portIndex = input_node().size(), creates port on input_node()
    REQUIRE( gAsNode->addLink( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 0 } ) );
    REQUIRE( gAsNode->addLink( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 1 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 1 );
    REQUIRE( gAsNode->getInputs().size() == 0 );
    REQUIRE( gAsNode->getOutputs().size() == 0 );

    gAsNode->generate_ports();

    REQUIRE( gAsNode->getInputs().size() == 1 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );

    DataflowGraph g { "mainGraph" };
    {
        auto sourceNodeA = g.addNode<Source>( "s" );
        auto resultNode  = g.addNode<Sink>( "r" );

        g.addNode( gAsNode );
        g.addLink( sourceNodeA->getOutPort().get(), gAsNode->getInputByIndex( 0 ) );
        g.addLink( gAsNode->getOutputByIndex( 0 ), resultNode->getInPort().get() );

        sourceNodeA->setData( 2 );
    }
    ///\todo make own test with serializable nodes and checks.
    std::string tmpdir { "tmpDir4Tests" };
    std::filesystem::create_directories( tmpdir );
    REQUIRE( g.shouldBeSaved() );
    g.saveToJson( tmpdir + "/graph_as_node_io.json" );
    REQUIRE( !g.shouldBeSaved() );

    // Create a new graph and load from the saved graph
    DataflowGraph g1 { "loaded graph" };
    g1.loadFromJson( tmpdir + "/graph_as_node_io.json" );

    {
        auto sourceNodeA = std::dynamic_pointer_cast<Source>( g1.getNode( "s" ) );

        REQUIRE( sourceNodeA );
        auto nodeGraph = std::dynamic_pointer_cast<DataflowGraph>( g1.getNode( "graphAsNode" ) );
        REQUIRE( nodeGraph->display_name() == "graphAsNode" );
        REQUIRE( nodeGraph );
        auto resultNode = std::dynamic_pointer_cast<Sink>( g1.getNode( "r" ) );
        REQUIRE( resultNode );

        REQUIRE( sourceNodeA->getData() );
        REQUIRE( *sourceNodeA->getData() == 2 );
    }
}

TEST_CASE( "Dataflow/Core/GraphAsNode/RemoveUnlinked", "[unittests][Dataflow][Core][Graph]" ) {

    using PortIndex    = Ra::Dataflow::Core::Node::PortIndex;
    using FunctionNode = Functionals::FunctionNode<Scalar>;
    using Source       = Sources::SingleDataSourceNode<Scalar>;
    using Sink         = Sinks::SinkNode<Scalar>;

    auto port_fatcory = PortFactory::createInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = make_shared<DataflowGraph>( "graphAsNode" );
    auto f       = gAsNode->addNode<FunctionNode>( "f" );

    gAsNode->add_input_output_nodes();

    REQUIRE( !gAsNode->canLink(
        gAsNode->input_node(), PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );

    // add link to portIndex = input_node().size(), creates port on input_node()
    REQUIRE( gAsNode->canLink( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 0 } ) );
    REQUIRE( !gAsNode->canLink( gAsNode->input_node(), PortIndex { 10 }, f, PortIndex { 0 } ) );
    REQUIRE( gAsNode->addLink( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 0 } ) );
    REQUIRE( gAsNode->canLink( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );
    REQUIRE( !gAsNode->canLink( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 1 } ) );
    REQUIRE( gAsNode->addLink( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 1 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 1 );
    REQUIRE( gAsNode->getInputs().size() == 0 );
    REQUIRE( gAsNode->getOutputs().size() == 0 );

    gAsNode->generate_ports();

    REQUIRE( gAsNode->getInputs().size() == 1 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );

    DataflowGraph g { "mainGraph" };

    auto sourceNodeA = g.addNode<Source>( "s" );
    auto resultNode  = g.addNode<Sink>( "r" );

    REQUIRE( g.addNode( gAsNode ) );
    REQUIRE( g.canLink( sourceNodeA, PortIndex { 0 }, gAsNode, PortIndex { 0 } ) );
    REQUIRE( g.addLink( sourceNodeA->getOutPort().get(), gAsNode->getInputByIndex( 0 ) ) );
    REQUIRE( g.addLink( gAsNode->getOutputByIndex( 0 ), resultNode->getInPort().get() ) );

    sourceNodeA->setData( 2 );

    REQUIRE( g.compile() );
    REQUIRE( g.execute() );

    // first setup
    gAsNode->remove_unlinked_input_output_ports();
    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 1 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 1 );
    REQUIRE( gAsNode->getInputs().size() == 1 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );

    // re link in gAsNode
    REQUIRE( gAsNode->removeLink( f, PortIndex { 0 } ) );
    REQUIRE( gAsNode->addLink( gAsNode->input_node(), PortIndex { 1 }, f, PortIndex { 0 } ) );
    REQUIRE( gAsNode->addLink( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 1 } ) );
    REQUIRE( gAsNode->removeLink( gAsNode->output_node(), PortIndex { 0 } ) );

    // input/output node of gAsNode still linked in g, remove unlink do nothing
    gAsNode->remove_unlinked_input_output_ports();

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 2 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 2 );
    REQUIRE( gAsNode->getInputs().size() == 2 );
    REQUIRE( gAsNode->getOutputs().size() == 2 );

    // unlinked in g, remove unlink clean up unused port in input/output nodes
    REQUIRE( g.removeLink( gAsNode->input_node(), PortIndex { 0 } ) );
    REQUIRE( g.removeLink( resultNode, PortIndex { 0 } ) );

    gAsNode->remove_unlinked_input_output_ports();

    // now cleaned
    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 1 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 1 );
    REQUIRE( gAsNode->getInputs().size() == 1 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );

    // relink
    std::cerr << "relink in g\n";
    REQUIRE( g.addLink( sourceNodeA->getOutPort().get(), gAsNode->getInputByIndex( 0 ) ) );
    REQUIRE( g.addLink( gAsNode->getOutputByIndex( 0 ), resultNode->getInPort().get() ) );

    gAsNode->remove_unlinked_input_output_ports();

    // no change
    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->getOutputs().size() == 1 );
    REQUIRE( gAsNode->output_node()->getInputs().size() == 1 );
    REQUIRE( gAsNode->getInputs().size() == 1 );
    REQUIRE( gAsNode->getOutputs().size() == 1 );
}
