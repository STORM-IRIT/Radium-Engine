#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/GraphNodes.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Sources/CoreDataSources.hpp>

void CoreNodes__Initializer();

struct CoreNodes__Initializer_t_ {
    CoreNodes__Initializer_t_() { ::CoreNodes__Initializer(); }
};
static CoreNodes__Initializer_t_ CoreNodes__Initializer__;

using namespace Ra::Dataflow::Core;

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

void CoreNodes__Initializer() {
    PortFactory::createInstance();
    using namespace Ra::Dataflow::Core::NodeFactoriesManager;
    if ( getFactory( getFactoryManager().default_factory_name() ) ) { return; }
    auto factory = createFactory( getFactoryManager().default_factory_name() );

    /* --- Functionals */
    /* --- Sources --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    REGISTER_TYPE_TO_FACTORY( factory, Sources::BooleanSource, Sources );
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
    REGISTER_TYPE_TO_FACTORY( factory, Sinks::BooleanSink, Sinks );

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) \
    ADD_SINKS_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )
    NODE_TYPES( factory, Sinks );
#undef DATAFLOW_MACRO

    /* --- Graphs --- */
    REGISTER_TYPE_TO_FACTORY( factory, DataflowGraph, Graph );
    REGISTER_TYPE_TO_FACTORY( factory, GraphInputNode, Graph );
    REGISTER_TYPE_TO_FACTORY( factory, GraphOutputNode, Graph );
}
