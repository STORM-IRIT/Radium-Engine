#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Functionals/Types.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/Core/Sinks/Types.hpp>
#include <Dataflow/Core/Sources/Types.hpp>

void CoreNodes__Initializer();

struct CoreNodes__Initializer_t_ {
    CoreNodes__Initializer_t_() { ::CoreNodes__Initializer(); }
};
static CoreNodes__Initializer_t_ CoreNodes__Initializer__;

using namespace Ra::Dataflow::Core;

#define ADD_TYPES_TO_FACTORY( FACTORY, FIX )                                              \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::ArrayFilter##FIX, Fonctionals );      \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::ArrayTransformer##FIX, Fonctionals ); \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::ArrayReducer##FIX, Fonctionals );     \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::BinaryOp##FIX, Fonctionals );         \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::BinaryOp##FIX##Array, Fonctionals );  \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::BinaryPredicate##FIX, Fonctionals );  \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Functionals::Transform##FIX, Fonctionals );        \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sinks::FIX##Sink, Sinks );                         \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sinks::FIX##ArraySink, Sinks );                    \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sources::FIX##Source, Sources );                   \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sources::FIX##ArraySource, Sources );              \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sources::FIX##UnaryFunctionSource, Sources );      \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sources::FIX##BinaryFunctionSource, Sources );     \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sources::FIX##UnaryPredicateSource, Sources );     \
    REGISTER_TYPE_TO_FACTORY( FACTORY, Sources::FIX##BinaryPredicateSource, Sources )

void CoreNodes__Initializer() {
    PortFactory::createInstance();
    using namespace Ra::Dataflow::Core::NodeFactoriesManager;
    if ( getFactory( getFactoryManager().default_factory_name() ) ) { return; }
    auto factory = createFactory( getFactoryManager().default_factory_name() );

    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    REGISTER_TYPE_TO_FACTORY( factory, Sources::BooleanSource, Sources );
    REGISTER_TYPE_TO_FACTORY( factory, Sinks::BooleanSink, Sinks );

#define DATAFLOW_MACRO( PREFIX, TYPE, FACTORY, NAMESPACE ) ADD_TYPES_TO_FACTORY( FACTORY, PREFIX )
    NODE_TYPES( factory, _ );
#undef DATAFLOW_MACRO

    /* --- Graphs --- */
    REGISTER_TYPE_TO_FACTORY( factory, DataflowGraph, Graph );
    REGISTER_TYPE_TO_FACTORY( factory, GraphInputNode, Graph );
    REGISTER_TYPE_TO_FACTORY( factory, GraphOutputNode, Graph );
}
