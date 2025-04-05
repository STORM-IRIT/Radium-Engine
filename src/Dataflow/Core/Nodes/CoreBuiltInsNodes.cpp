#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/FunctionalsNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SinksNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SourcesNodeFactory.hpp>
#include <memory>
#include <string>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

void registerStandardFactories() {
    if ( getFactory( getFactoryManager().default_factory_name() ) ) { return; }
    auto coreFactory = createFactory( getFactoryManager().default_factory_name() );
    /* --- Sources --- */
    Private::registerSourcesFactories( coreFactory );

    /* --- Sinks --- */
    Private::registerSinksFactories( coreFactory );

    /* --- Functionals */
    Private::registerFunctionalsFactories( coreFactory );

    /* --- Graphs --- */
    coreFactory->registerNodeCreator<DataflowGraph>( DataflowGraph::getTypename() + "_", "Graph" );
    coreFactory->registerNodeCreator<GraphInputNode>( GraphInputNode::getTypename() + "_",
                                                      "Graph" );
    coreFactory->registerNodeCreator<GraphOutputNode>( GraphOutputNode::getTypename() + "_",
                                                       "Graph" );
}
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
