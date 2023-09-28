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

    forwardA->getInPort()->setName( "a" );
    forwardB->getInPort()->setName( "b" );
    forwardC->getInPort()->setName( "c" );
    b2minus4ac->getOutputPort()->setName( "delta" );

    auto inputA = gAsNode->addInput( forwardA->getInPort() );
    auto inputB = gAsNode->addInput( forwardB->getInPort() );
    auto inputC = gAsNode->addInput( forwardC->getInPort() );
    auto output = gAsNode->addOutput( b2minus4ac->getOutputPort() );

    gAsNode->addLink( forwardB->getOutPort(), b2->getInPort() );
    gAsNode->addLink( forwardA->getOutPort(), fourAC->getPortA() );
    gAsNode->addLink( forwardC->getOutPort(), fourAC->getPortB() );

    gAsNode->addLink( b2->getOutPort(), b2minus4ac->getPortA() );
    gAsNode->addLink( fourAC->getOutputPort(), b2minus4ac->getPortB() );

    std::cout << "== Graph as node ==\n";
    std::cout << "inputs\n";
    for ( const auto& n : gAsNode->getInputs() ) {
        std::cout << "\t- " << n->getName() << " <" << n->getTypeName() << ">\n";
    }

    std::cout << "outputs\n";
    for ( const auto& n : gAsNode->getOutputs() ) {
        std::cout << "\t- " << n->getName() << " <" << n->getTypeName() << ">\n";
    }

    DataflowGraph g { "mainGraph" };
    auto sourceNodeA = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "a" );
    auto sourceNodeB = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "b" );
    auto sourceNodeC = g.addNode<Sources::SingleDataSourceNode<Scalar>>( "c" );

    auto resultNode = g.addNode<Sinks::SinkNode<Scalar>>( "delta" );

    g.addNode( gAsNode );

    g.addLink( sourceNodeA->getOuputPort().get(), gAsNode->getInputByIndex<Scalar>( inputA ) );
    g.addLink( sourceNodeB->getOuputPort().get(), gAsNode->getInputByIndex<Scalar>( inputB ) );
    g.addLink( sourceNodeC->getOuputPort().get(), gAsNode->getInputByIndex<Scalar>( inputC ) );
    g.addLink( gAsNode->getOutputByIndex<Scalar>( output ), resultNode->getInPort().get() );

    sourceNodeA->setData( 1 );
    sourceNodeB->setData( 2 );
    sourceNodeC->setData( 3 );

    if ( !g.compile() ) {
        std::cout << " compilation failed";
        return 1;
    }

    //! [Verifying the graph can be compiled]
    g.saveToJson( "illustrateDoc.json" );

    g.execute();

    auto& result = resultNode->getDataByRef();
    std::cout << "Output value delta = " << *sourceNodeB->getData() << "²-4×"
              << *sourceNodeA->getData() << "×" << *sourceNodeC->getData() << " = " << result
              << "\n";

    return 0;
}
