#include <Core/Containers/MakeShared.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/BinaryOpNode.hpp>
#include <Dataflow/Core/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

using namespace Ra::Dataflow::Core;

/* ----------------------------------------------------------------------------------- */
/**
 * \brief Demonstrate how to use a graph to filter a collection
 */
int main( int argc, char* argv[] ) {
    using RaVector = Ra::Core::VectorArray<Scalar>;

    auto port_fatcory = PortFactory::getInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = Ra::Core::make_shared<DataflowGraph>( "graphAsNode" );

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

    gAsNode->add_input_output_nodes();
    auto inputA = gAsNode->input_node()->add_output_port( forwardA->port_in_data().get() );
    auto inputB = gAsNode->input_node()->add_output_port( forwardB->port_in_data().get() );
    auto inputC = gAsNode->input_node()->add_output_port( forwardC->port_in_data().get() );
    auto output = gAsNode->output_node()->add_input_port( b2minus4ac->port_out_result().get() );

    gAsNode->input_node()->input_by_index( inputA )->set_name( "a" );
    gAsNode->input_node()->input_by_index( inputB )->set_name( "b" );
    gAsNode->input_node()->input_by_index( inputC )->set_name( "c" );
    gAsNode->output_node()->output_by_index( output )->set_name( "delta" );

    gAsNode->add_link( forwardB->port_out_result(), b2->port_in_data() );
    gAsNode->add_link( forwardA->port_out_result(), fourAC->port_in_a() );
    gAsNode->add_link( forwardC->port_out_result(), fourAC->port_in_b() );

    gAsNode->add_link( b2->port_out_result(), b2minus4ac->port_in_a() );
    gAsNode->add_link( fourAC->port_out_result(), b2minus4ac->port_in_b() );

    gAsNode->compile();
    std::cout << "== Graph as node ==\n";
    std::cout << "inputs\n";
    for ( const auto& n : gAsNode->inputs() ) {
        std::cout << "\t- " << n->name() << " <" << n->port_typename() << "> ("
                  << n->node()->display_name() << ")\n";
    }

    std::cout << "outputs\n";
    for ( const auto& n : gAsNode->outputs() ) {
        std::cout << "\t- " << n->name() << " <" << n->port_typename() << "> ("
                  << n->node()->display_name() << ")\n";
    }

    DataflowGraph g { "mainGraph" };
    auto sourceNodeA = g.add_node<Sources::SingleDataSourceNode<Scalar>>( "sa" );
    auto sourceNodeB = g.add_node<Sources::SingleDataSourceNode<Scalar>>( "sb" );
    auto sourceNodeC = g.add_node<Sources::SingleDataSourceNode<Scalar>>( "sc" );

    auto resultNode = g.add_node<Sinks::SinkNode<Scalar>>( "odelta" );

    g.add_node( gAsNode );

    g.add_link( sourceNodeA->port_out_to().get(), gAsNode->input_by_index( inputA ) );
    g.add_link( sourceNodeB->port_out_to().get(), gAsNode->input_by_index( inputB ) );
    g.add_link( sourceNodeC->port_out_to().get(), gAsNode->input_by_index( inputC ) );
    g.add_link( gAsNode->output_by_index( output ), resultNode->port_in_from().get() );

    sourceNodeA->set_data( 1 );
    sourceNodeB->set_data( 2 );
    sourceNodeC->set_data( 3 );

    auto p = resultNode->port_in_from();
    std::cerr << p->name() << " " << p->is_linked() << "\n";
    std::cerr << p->link()->name() << "\n";
    if ( !g.compile() ) {
        std::cout << " compilation failed\n";
        return 1;
    }
    std::cerr << "compile ok\n";

    g.saveToJson( "illustrateDoc.json" );

    if ( !g.execute() ) {
        std::cout << " execution failed\n";
        return 1;
    }

    auto& result = resultNode->data_reference();
    std::cout << "Output value delta = " << *sourceNodeB->data() << "²-4×" << *sourceNodeA->data()
              << "×" << *sourceNodeC->data() << " = " << result << "\n";
    if ( result != ( 2 * 2 ) - 4 * 1 * 3 ) return 1;
    return 0;
}
