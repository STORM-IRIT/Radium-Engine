

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Core/Nodes/Private/FunctionalsNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SinksNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SourcesNodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

void registerStandardFactories() {
    NodeFactorySet::mapped_type coreFactory { new NodeFactorySet::mapped_type::element_type(
        NodeFactoriesManager::dataFlowBuiltInsFactoryName ) };
    /* --- Sources --- */
    Private::registerSourcesFactories( coreFactory );

    /* --- Sinks --- */
    Private::registerSinksFactories( coreFactory );

    /* --- Functionals */
    Private::registerFunctionalsFactories( coreFactory );

    /* --- Graphs --- */
    coreFactory->registerNodeCreator<DataflowGraph>( DataflowGraph::getTypename() + "_", "Graph" );

    registerFactory( coreFactory );
}
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra

DATAFLOW_LIBRARY_INITIALIZER( CoreNodes ) {
    Ra::Dataflow::Core::NodeFactoriesManager::registerStandardFactories();
}
