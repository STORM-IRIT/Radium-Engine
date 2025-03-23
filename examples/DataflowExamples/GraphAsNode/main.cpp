#include <Core/Containers/MakeShared.hpp>
#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Nodes/Functionals/BinaryOpNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/FilterNode.hpp>
#include <Dataflow/Core/Nodes/Functionals/FunctionNode.hpp>
#include <Dataflow/Core/Nodes/Sinks/SinkNode.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

using namespace Ra::Dataflow::Core;

/* ----------------------------------------------------------------------------------- */
/**
 * \brief Demonstrate how to use a graph to filter a collection
 */
int main( int argc, char* argv[] ) {
    using RaVector = Ra::Core::VectorArray<Scalar>;

    auto port_fatcory = PortFactory::createInstance();
    port_fatcory->add_port_type<Scalar>();

    auto gAsNode = Ra::Core::make_shared<DataflowGraph>( "graphAsNode" );

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

    gAsNode->add_input_output_nodes();
    auto inputA = gAsNode->input_node()->add_output_port( forwardA->getInPort() );
    auto inputB = gAsNode->input_node()->add_output_port( forwardB->getInPort() );
    auto inputC = gAsNode->input_node()->add_output_port( forwardC->getInPort() );
    auto output = gAsNode->output_node()->add_input_port( b2minus4ac->getOutputPort() );

    gAsNode->input_node()->getInputByIndex( inputA )->setName( "a" );
    gAsNode->input_node()->getInputByIndex( inputB )->setName( "b" );
    gAsNode->input_node()->getInputByIndex( inputC )->setName( "c" );
    gAsNode->output_node()->getOutputByIndex( output )->setName( "delta" );

    gAsNode->addLink( forwardB->getOutPort(), b2->getInPort() );
    gAsNode->addLink( forwardA->getOutPort(), fourAC->getPortA() );
    gAsNode->addLink( forwardC->getOutPort(), fourAC->getPortB() );

    gAsNode->addLink( b2->getOutPort(), b2minus4ac->getPortA() );
    gAsNode->addLink( fourAC->getOutputPort(), b2minus4ac->getPortB() );

    gAsNode->compile();
    std::cout << "== Graph as node ==\n";
    std::cout << "inputs\n";
    for ( const auto& n : gAsNode->getInputs() ) {
        std::cout << "\t- " << n->getName() << " <" << n->getTypeName() << "> ("
                  << n->getNode()->display_name() << ")\n";
    }

    std::cout << "outputs\n";
    for ( const auto& n : gAsNode->getOutputs() ) {
        std::cout << "\t- " << n->getName() << " <" << n->getTypeName() << "> ("
                  << n->getNode()->display_name() << ")\n";
    }

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
    std::cerr << p->getName() << " " << p->isLinked() << "\n";
    std::cerr << p->getLink()->getName() << "\n";
    if ( !g.compile() ) {
        std::cout << " compilation failed\n";
        return 1;
    }
    std::cerr << "compile ok\n";
    //! [Verifying the graph can be compiled]
    // g.saveToJson( "illustrateDoc.json" );

    if ( g.execute() ) {
        std::cout << " execution failed\n";
        //  return 1;
    }

    auto& result = resultNode->getDataByRef();
    std::cout << "Output value delta = " << *sourceNodeB->getData() << "²-4×"
              << *sourceNodeA->getData() << "×" << *sourceNodeC->getData() << " = " << result
              << "\n";

    return 0;
}
