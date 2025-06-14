#include <catch2/catch_test_macros.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/BinaryOpNode.hpp>
#include <Dataflow/Core/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Functionals/ReduceNode.hpp>
#include <Dataflow/Core/Functionals/TransformNode.hpp>
#include <Dataflow/Core/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Sinks/Types.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

using namespace Ra::Dataflow::Core;
using namespace Ra::Core;

TEST_CASE( "Dataflow/Core/GraphAsNode/Delta", "[unittests][Dataflow][Core][Graph]" ) {
    auto port_fatcory = PortFactory::getInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = make_shared<DataflowGraph>( "graphAsNode" );

    // compute delta = b2 - 4ac;

    auto b2 = gAsNode->add_node<Functionals::FunctionNode<Scalar>>( "b2" );
    b2->set_function( []( const Scalar& b ) { return b * b; } );

    auto fourAC = gAsNode->add_node<Functionals::BinaryOpNode<Scalar>>(
        "4ac", []( const Scalar& a, const Scalar& c ) { return 4_ra * a * c; } );

    auto b2minus4ac = gAsNode->add_node<Functionals::BinaryOpNode<Scalar>>(
        "b2-4ac", []( const Scalar& x, const Scalar& y ) { return x - y; } );

    auto forwardA = gAsNode->add_node<Functionals::FunctionNode<Scalar>>( "a" );
    auto forwardB = gAsNode->add_node<Functionals::FunctionNode<Scalar>>( "b" );
    auto forwardC = gAsNode->add_node<Functionals::FunctionNode<Scalar>>( "c" );

    b2minus4ac->port_out_result()->set_name( "delta" );

    REQUIRE( !gAsNode->input_node() );
    REQUIRE( !gAsNode->output_node() );

    gAsNode->add_input_output_nodes();
    auto inputA = gAsNode->input_node()->add_output_port( forwardA->port_in_data().get() );
    auto inputB = gAsNode->input_node()->add_output_port( forwardB->port_in_data().get() );
    auto inputC = gAsNode->input_node()->add_output_port( forwardC->port_in_data().get() );
    auto output = gAsNode->output_node()->add_input_port( b2minus4ac->port_out_result().get() );

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->outputs().size() == 3 );
    REQUIRE( gAsNode->output_node()->inputs().size() == 1 );
    REQUIRE( gAsNode->inputs().size() == 0 );
    REQUIRE( gAsNode->outputs().size() == 0 );

    gAsNode->input_node()->input_by_index( inputA )->set_name( "a" );
    gAsNode->input_node()->input_by_index( inputB )->set_name( "b" );
    gAsNode->input_node()->input_by_index( inputC )->set_name( "c" );
    gAsNode->output_node()->output_by_index( output )->set_name( "delta" );

    REQUIRE( gAsNode->add_link( forwardB->port_out_result(), b2->port_in_data() ) );
    REQUIRE( gAsNode->add_link( forwardA->port_out_result(), fourAC->port_in_a() ) );
    REQUIRE( gAsNode->add_link( forwardC->port_out_result(), fourAC->port_in_b() ) );

    REQUIRE( gAsNode->add_link( b2->port_out_result(), b2minus4ac->port_in_a() ) );
    REQUIRE( gAsNode->add_link( fourAC->port_out_result(), b2minus4ac->port_in_b() ) );

    REQUIRE( gAsNode->compile() );

    REQUIRE( gAsNode->inputs().size() == 3 );
    REQUIRE( gAsNode->outputs().size() == 1 );

    DataflowGraph g { "mainGraph" };
    auto sourceNodeA = g.add_node<Sources::SingleDataSourceNode<Scalar>>( "sa" );
    auto sourceNodeB = g.add_node<Sources::SingleDataSourceNode<Scalar>>( "sb" );
    auto sourceNodeC = g.add_node<Sources::SingleDataSourceNode<Scalar>>( "sc" );

    auto resultNode = g.add_node<Sinks::SinkNode<Scalar>>( "odelta" );

    REQUIRE( g.add_node( gAsNode ) );

    REQUIRE( g.add_link( sourceNodeA->port_out_to().get(), gAsNode->input_by_index( inputA ) ) );
    REQUIRE( g.add_link( sourceNodeB->port_out_to().get(), gAsNode->input_by_index( inputB ) ) );
    REQUIRE( g.add_link( sourceNodeC->port_out_to().get(), gAsNode->input_by_index( inputC ) ) );
    REQUIRE( g.add_link( gAsNode->output_by_index( output ), resultNode->port_in_from().get() ) );

    sourceNodeA->set_data( 1 );
    sourceNodeB->set_data( 2 );
    sourceNodeC->set_data( 3 );

    REQUIRE( g.compile() );
    REQUIRE( g.execute() );

    auto& result = resultNode->data_reference();
    REQUIRE( result == -8 );
}

using PortIndex    = Ra::Dataflow::Core::Node::PortIndex;
using FunctionNode = Functionals::FunctionNode<Scalar>;
using Source       = Sources::SingleDataSourceNode<Scalar>;
using Sink         = Sinks::SinkNode<Scalar>;

TEST_CASE( "Dataflow/Core/GraphAsNode/Forward", "[unittests][Dataflow][Core][Graph]" ) {

    auto port_fatcory = PortFactory::createInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = make_shared<DataflowGraph>( "graphAsNode" );
    auto f       = gAsNode->add_node<FunctionNode>( "f" );

    REQUIRE( !gAsNode->input_node() );
    REQUIRE( !gAsNode->output_node() );

    gAsNode->add_input_output_nodes();

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );

    REQUIRE( !gAsNode->can_link(
        gAsNode->input_node(), PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );
    REQUIRE( !gAsNode->add_link(
        gAsNode->input_node(), PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );

    // add link to portIndex = input_node().size(), creates port on input_node()
    REQUIRE( !gAsNode->can_link( gAsNode->input_node(), PortIndex { 10 }, f, PortIndex { 0 } ) );
    REQUIRE( !gAsNode->can_link( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 10 } ) );
    REQUIRE( gAsNode->can_link( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 0 } ) );

    REQUIRE( !gAsNode->add_link( gAsNode->input_node(), PortIndex { 10 }, f, PortIndex { 0 } ) );
    REQUIRE( !gAsNode->add_link( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 10 } ) );
    REQUIRE( gAsNode->add_link( gAsNode->input_node(), PortIndex { 0 }, f, PortIndex { 0 } ) );

    REQUIRE( !gAsNode->can_link( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 1 } ) );
    REQUIRE( !gAsNode->can_link( f, PortIndex { 1 }, gAsNode->output_node(), PortIndex { 0 } ) );
    REQUIRE( gAsNode->can_link( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );

    REQUIRE( !gAsNode->add_link( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 1 } ) );
    REQUIRE( !gAsNode->add_link( f, PortIndex { 1 }, gAsNode->output_node(), PortIndex { 0 } ) );
    REQUIRE( gAsNode->add_link( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 0 } ) );

    REQUIRE( gAsNode->input_node() );
    REQUIRE( gAsNode->output_node() );
    REQUIRE( gAsNode->input_node()->outputs().size() == 1 );
    REQUIRE( gAsNode->output_node()->inputs().size() == 1 );
    REQUIRE( gAsNode->inputs().size() == 0 );
    REQUIRE( gAsNode->outputs().size() == 0 );

    gAsNode->generate_ports();

    REQUIRE( gAsNode->inputs().size() == 1 );
    REQUIRE( gAsNode->outputs().size() == 1 );

    DataflowGraph g { "mainGraph" };

    auto sourceNodeA = g.add_node<Source>( "s" );
    auto resultNode  = g.add_node<Sink>( "r" );

    REQUIRE( g.add_node( gAsNode ) );
    REQUIRE( g.can_link( sourceNodeA, PortIndex { 0 }, gAsNode, PortIndex { 0 } ) );
    REQUIRE( g.add_link( sourceNodeA->port_out_to().get(), gAsNode->input_by_index( 0 ) ) );
    REQUIRE( g.add_link( gAsNode->output_by_index( 0 ), resultNode->port_in_from().get() ) );

    sourceNodeA->set_data( 2 );

    SECTION( "Serialization" ) {

        ///\todo make own test with serializable nodes and checks.
        std::string tmpdir { "tmpDir4Tests" };
        std::filesystem::create_directories( tmpdir );
        REQUIRE( g.shouldBeSaved() );
        g.saveToJson( tmpdir + "/graph_as_node_io.json" );
        REQUIRE( !g.shouldBeSaved() );

        // Create a new graph and load from the saved graph
        DataflowGraph g1 { "loaded graph" };
        REQUIRE( g1.loadFromJson( tmpdir + "/graph_as_node_io.json" ) );
        {
            auto g1_sourceNodeA = std::dynamic_pointer_cast<Source>( g1.node( "s" ) );

            REQUIRE( g1_sourceNodeA );
            auto g1_nodeGraph =
                std::dynamic_pointer_cast<DataflowGraph>( g1.node( "graphAsNode" ) );
            REQUIRE( g1_nodeGraph->display_name() == "graphAsNode" );
            REQUIRE( g1_nodeGraph );
            auto g1_resultNode = std::dynamic_pointer_cast<Sink>( g1.node( "r" ) );
            REQUIRE( g1_resultNode );

            REQUIRE( g1_sourceNodeA->data() );
            REQUIRE( *g1_sourceNodeA->data() == 2 );
        }
    }

    SECTION( "Remove unlinked" ) {

        REQUIRE( g.compile() );
        REQUIRE( g.execute() );

        // first setup
        gAsNode->remove_unlinked_input_output_ports();
        REQUIRE( gAsNode->input_node() );
        REQUIRE( gAsNode->output_node() );
        REQUIRE( gAsNode->input_node()->outputs().size() == 1 );
        REQUIRE( gAsNode->output_node()->inputs().size() == 1 );
        REQUIRE( gAsNode->inputs().size() == 1 );
        REQUIRE( gAsNode->outputs().size() == 1 );

        // re link in gAsNode
        REQUIRE( gAsNode->remove_link( f, PortIndex { 0 } ) );
        REQUIRE( gAsNode->add_link( gAsNode->input_node(), PortIndex { 1 }, f, PortIndex { 0 } ) );
        REQUIRE( gAsNode->add_link( f, PortIndex { 0 }, gAsNode->output_node(), PortIndex { 1 } ) );
        REQUIRE( gAsNode->remove_link( gAsNode->output_node(), PortIndex { 0 } ) );

        // input/output node of gAsNode still linked in g, remove unlink do nothing
        gAsNode->remove_unlinked_input_output_ports();

        REQUIRE( gAsNode->input_node() );
        REQUIRE( gAsNode->output_node() );
        REQUIRE( gAsNode->input_node()->outputs().size() == 2 );
        REQUIRE( gAsNode->output_node()->inputs().size() == 2 );
        REQUIRE( gAsNode->inputs().size() == 2 );
        REQUIRE( gAsNode->outputs().size() == 2 );

        // unlinked in g, remove unlink clean up unused port in input/output nodes
        REQUIRE( g.remove_link( gAsNode->input_node(), PortIndex { 0 } ) );
        REQUIRE( g.remove_link( resultNode, PortIndex { 0 } ) );

        gAsNode->remove_unlinked_input_output_ports();

        // now cleaned
        REQUIRE( gAsNode->input_node() );
        REQUIRE( gAsNode->output_node() );
        REQUIRE( gAsNode->input_node()->outputs().size() == 1 );
        REQUIRE( gAsNode->output_node()->inputs().size() == 1 );
        REQUIRE( gAsNode->inputs().size() == 1 );
        REQUIRE( gAsNode->outputs().size() == 1 );

        // relink
        REQUIRE( g.add_link( sourceNodeA->port_out_to().get(), gAsNode->input_by_index( 0 ) ) );
        REQUIRE( g.add_link( gAsNode->output_by_index( 0 ), resultNode->port_in_from().get() ) );

        gAsNode->remove_unlinked_input_output_ports();

        // no change
        REQUIRE( gAsNode->input_node() );
        REQUIRE( gAsNode->output_node() );
        REQUIRE( gAsNode->input_node()->outputs().size() == 1 );
        REQUIRE( gAsNode->output_node()->inputs().size() == 1 );
        REQUIRE( gAsNode->inputs().size() == 1 );
        REQUIRE( gAsNode->outputs().size() == 1 );
    }
}
