#include <Dataflow/Core/DataflowGraph.hpp>
DATAFLOW_LIBRARY_INITIALIZER_DECL( CoreNodes );

#include <Dataflow/Core/Nodes/Private/FunctionalsNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SinksNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SourcesNodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

void registerStandardFactories() {
    auto coreFactory = createFactory( NodeFactoriesManager::dataFlowBuiltInsFactoryName );
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

DATAFLOW_LIBRARY_INITIALIZER_IMPL( CoreNodes ) {
    Ra::Dataflow::Core::NodeFactoriesManager::registerStandardFactories();
}
