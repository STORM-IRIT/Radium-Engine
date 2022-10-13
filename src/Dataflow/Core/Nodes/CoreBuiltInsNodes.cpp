#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Core/NodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

/** TODO : replace this by factory autoregistration at compile time */
#define ADD_FUNCTIONALS_TO_FACTORY( FACTORY, NAMESPACE, SUFFIX )                \
    FACTORY->registerNodeCreator<NAMESPACE::ArrayFilter##SUFFIX>(               \
        NAMESPACE::ArrayFilter##SUFFIX::getTypename() + "_", #NAMESPACE );      \
    FACTORY->registerNodeCreator<NAMESPACE::ArrayTransformer##SUFFIX>(          \
        NAMESPACE::ArrayTransformer##SUFFIX::getTypename() + "_", #NAMESPACE ); \
    FACTORY->registerNodeCreator<NAMESPACE::ArrayReducer##SUFFIX>(              \
        NAMESPACE::ArrayReducer##SUFFIX::getTypename() + "_", #NAMESPACE );     \
    FACTORY->registerNodeCreator<NAMESPACE::BinaryOp##SUFFIX>(                  \
        NAMESPACE::BinaryOp##SUFFIX::getTypename() + "_", #NAMESPACE );         \
    FACTORY->registerNodeCreator<NAMESPACE::BinaryOp##SUFFIX##Array>(           \
        NAMESPACE::BinaryOp##SUFFIX##Array::getTypename() + "_", #NAMESPACE );  \
    FACTORY->registerNodeCreator<NAMESPACE::BinaryPredicate##SUFFIX>(           \
        NAMESPACE::BinaryPredicate##SUFFIX::getTypename() + "_", #NAMESPACE )

/*
 * not yet supported
    FACTORY->registerNodeCreator<NAMESPACE::BinaryPredicate##SUFFIX##Array>(    \
        NAMESPACE::BinaryPredicate##SUFFIX##Array::getTypename() + "_", #NAMESPACE )
*/

#define ADD_SOURCES_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )                        \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##Source>(                        \
        NAMESPACE::PREFIX##Source::getTypename() + "_", #NAMESPACE );               \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##ArraySource>(                   \
        NAMESPACE::PREFIX##ArraySource::getTypename() + "_", #NAMESPACE );          \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##UnaryFunctionSource>(           \
        NAMESPACE::PREFIX##UnaryFunctionSource::getTypename() + "_", #NAMESPACE );  \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##BinaryFunctionSource>(          \
        NAMESPACE::PREFIX##BinaryFunctionSource::getTypename() + "_", #NAMESPACE ); \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##UnaryPredicateSource>(          \
        NAMESPACE::PREFIX##UnaryPredicateSource::getTypename() + "_", #NAMESPACE ); \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##BinaryPredicateSource>(         \
        NAMESPACE::PREFIX##BinaryPredicateSource::getTypename() + "_", #NAMESPACE )

#define ADD_SINKS_TO_FACTORY( FACTORY, NAMESPACE, PREFIX )          \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##Sink>(          \
        NAMESPACE::PREFIX##Sink::getTypename() + "_", #NAMESPACE ); \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##ArraySink>(     \
        NAMESPACE::PREFIX##ArraySink::getTypename() + "_", #NAMESPACE )

void registerStandardFactories() {
    NodeFactorySet::mapped_type coreFactory { new NodeFactorySet::mapped_type::element_type(
        NodeFactoriesManager::dataFlowBuiltInsFactoryName ) };

    /* --- Sources --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    coreFactory->registerNodeCreator<Sources::BooleanSource>(
        Sources::BooleanSource::getTypename() + "_", "Sources" );
    // prevent Scalar type collision with float or double in factory
#ifdef CORE_USE_DOUBLE
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Float );
#else
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Double );
#endif
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Scalar );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Int );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, UInt );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Color );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector2f );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector2d );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector3f );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector3d );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector4f );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector4d );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector2i );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector2ui );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector3i );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector3ui );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector4i );
    ADD_SOURCES_TO_FACTORY( coreFactory, Sources, Vector4ui );

    /* --- Sinks --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    coreFactory->registerNodeCreator<Sinks::BooleanSink>( Sinks::BooleanSink::getTypename() + "_",
                                                          "Sinks" );
#ifdef CORE_USE_DOUBLE
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Float );
#else
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Double );
#endif
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Scalar );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Int );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, UInt );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Color );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector2f );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector2d );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector3f );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector3d );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector4f );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector4d );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector2i );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector2ui );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector3i );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector3ui );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector4i );
    ADD_SINKS_TO_FACTORY( coreFactory, Sinks, Vector4ui );

    /* --- Functionals */
#ifdef CORE_USE_DOUBLE
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Float );
#else
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Double );
#endif
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Scalar );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Int );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, UInt );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Color );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector2f );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector2d );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector3f );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector3d );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector4f );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector4d );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector2i );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector2ui );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector3i );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector3ui );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector4i );
    ADD_FUNCTIONALS_TO_FACTORY( coreFactory, Functionals, Vector4ui );

    /* --- Graphs --- */
    coreFactory->registerNodeCreator<DataflowGraph>( DataflowGraph::getTypename() + "_", "Graph" );

    registerFactory( coreFactory );
}
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
