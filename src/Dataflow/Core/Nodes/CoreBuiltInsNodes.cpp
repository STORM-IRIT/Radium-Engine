#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/Nodes/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>
#include <memory>
#include <string>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace NodeFactoriesManager {

/** TODO : replace this by factory autoregistration at compile time */
#define ADD_FUNCTIONALS_TO_FACTORY( FACTORY, NAMESPACE, SUFFIX )                         \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::ArrayFilter##SUFFIX, NAMESPACE );      \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::ArrayTransformer##SUFFIX, NAMESPACE ); \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::ArrayReducer##SUFFIX, NAMESPACE );     \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::BinaryOp##SUFFIX, NAMESPACE );         \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::BinaryOp##SUFFIX##Array, NAMESPACE );  \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::BinaryPredicate##SUFFIX, NAMESPACE );  \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::Transform##SUFFIX, NAMESPACE )

#define ADD_SINKS_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )                   \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##Sink, NAMESPACE ); \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##ArraySink, NAMESPACE )

#define ADD_SOURCES_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )                                 \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##Source, NAMESPACE );               \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##ArraySource, NAMESPACE );          \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##UnaryFunctionSource, NAMESPACE );  \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##BinaryFunctionSource, NAMESPACE ); \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##UnaryPredicateSource, NAMESPACE ); \
    REGISTER_TYPE_TO_FACTORY( FACTORY, NAMESPACE::PREFIX##BinaryPredicateSource, NAMESPACE )

void registerStandardFactories() {
    if ( getFactory( getFactoryManager().default_factory_name() ) ) { return; }
    auto factory = createFactory( getFactoryManager().default_factory_name() );

    /* --- Functionals */
    /* --- Sources --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    factory->registerNodeCreator<Sources::BooleanSource>(
        Sources::BooleanSource::getTypename() + "_", "Sources" );
    // prevent Scalar type collision with float or double in factory

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) \
    ADD_SOURCES_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )
    NODE_TYPES( factory, Sources );
#undef DATAFLOW_MACRO

    /* --- Functionals */
#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) \
    ADD_FUNCTIONALS_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )
    NODE_TYPES( factory, Functionals );
#undef DATAFLOW_MACRO

    /* --- Sinks --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    factory->registerNodeCreator<Sinks::BooleanSink>( Sinks::BooleanSink::getTypename() + "_",
                                                      "Sinks" );

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) \
    ADD_SINKS_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )
    NODE_TYPES( factory, Sinks );
#undef DATAFLOW_MACRO

    /* --- Graphs --- */
    factory->registerNodeCreator<DataflowGraph>( DataflowGraph::getTypename() + "_", "Graph" );
    factory->registerNodeCreator<GraphInputNode>( GraphInputNode::getTypename() + "_", "Graph" );
    factory->registerNodeCreator<GraphOutputNode>( GraphOutputNode::getTypename() + "_", "Graph" );
}
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
