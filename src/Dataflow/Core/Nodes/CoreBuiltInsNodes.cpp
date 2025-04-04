#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/FunctionalsNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SinksNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Private/SourcesNodeFactory.hpp>
#include <memory>
#include <string>

/// Allow to define initializers for modules that need to be initialized transparently
#define DATAFLOW_LIBRARY_INITIALIZER_DECL( f ) void f##__Initializer()

#define DATAFLOW_LIBRARY_INITIALIZER_IMPL( f )     \
    struct f##__Initializer_t_ {                   \
        f##__Initializer_t_() {                    \
            ::f##__Initializer();                  \
        }                                          \
    };                                             \
    static f##__Initializer_t_ f##__Initializer__; \
    void f##__Initializer()

DATAFLOW_LIBRARY_INITIALIZER_DECL( CoreNodes );

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

void registerStandardFactories() {
    if ( getFactory( "Dataflow Nodes" ) ) { return; }
    auto coreFactory = createFactory( "Dataflow Nodes" );
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

DATAFLOW_LIBRARY_INITIALIZER_IMPL( CoreNodes ) {
    Ra::Dataflow::Core::NodeFactoriesManager::registerStandardFactories();
}
