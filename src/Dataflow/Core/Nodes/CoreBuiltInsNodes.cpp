#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Core/NodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {
/** TODO : replace this by factory autoregistration at compile time */
#define ADD_TO_FACTORY( FACTORY, NAMESPACE, SUFFIX )                       \
    FACTORY->registerNodeCreator<NAMESPACE::ArrayFilter##SUFFIX>(          \
        NAMESPACE::ArrayFilter##SUFFIX::getTypename() + "_", #NAMESPACE ); \
    FACTORY->registerNodeCreator<NAMESPACE::ArrayMapper##SUFFIX>(          \
        NAMESPACE::ArrayMapper##SUFFIX::getTypename() + "_", #NAMESPACE )

void registerStandardFactories() {
    NodeFactorySet::mapped_type coreFactory { new NodeFactorySet::mapped_type::element_type(
        NodeFactoriesManager::dataFlowBuiltInsFactoryName ) };

    /* --- Sources --- */
    coreFactory->registerNodeCreator<Sources::BooleanValueSource>(
        Sources::BooleanValueSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::IntValueSource>(
        Sources::IntValueSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::UIntValueSource>(
        Sources::UIntValueSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::ScalarValueSource>(
        Sources::ScalarValueSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::ColorSourceNode>(
        Sources::ColorSourceNode::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::FloatArrayDataSource>(
        Sources::FloatArrayDataSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::DoubleArrayDataSource>(
        Sources::DoubleArrayDataSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::IntArrayDataSource>(
        Sources::IntArrayDataSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::UIntArrayDataSource>(
        Sources::UIntArrayDataSource::getTypename() + "_", "Source" );
    coreFactory->registerNodeCreator<Sources::ColorArrayDataSource>(
        Sources::ColorArrayDataSource::getTypename() + "_", "Source" );

    /* --- Sinks --- */
    coreFactory->registerNodeCreator<Sinks::BooleanSink>( Sinks::BooleanSink::getTypename() + "_",
                                                          "Sink" );
    coreFactory->registerNodeCreator<Sinks::IntSink>( Sinks::IntSink::getTypename() + "_", "Sink" );
    coreFactory->registerNodeCreator<Sinks::UIntSink>( Sinks::UIntSink::getTypename() + "_",
                                                       "Sink" );
    coreFactory->registerNodeCreator<Sinks::ScalarSink>( Sinks::ScalarSink::getTypename() + "_",
                                                         "Sink" );
    coreFactory->registerNodeCreator<Sinks::ColorSink>( Sinks::ColorSink::getTypename() + "_",
                                                        "Sink" );
    coreFactory->registerNodeCreator<Sinks::FloatArraySink>(
        Sinks::FloatArraySink::getTypename() + "_", "Sink" );
    coreFactory->registerNodeCreator<Sinks::DoubleArraySink>(
        Sinks::DoubleArraySink::getTypename() + "_", "Sink" );
    coreFactory->registerNodeCreator<Sinks::IntArraySink>( Sinks::IntArraySink::getTypename() + "_",
                                                           "Sink" );
    coreFactory->registerNodeCreator<Sinks::UIntArraySink>(
        Sinks::UIntArraySink::getTypename() + "_", "Sink" );
    coreFactory->registerNodeCreator<Sinks::ColorArraySink>(
        Sinks::ColorArraySink::getTypename() + "_", "Sink" );

    /* --- Functionals */
    ADD_TO_FACTORY( coreFactory, Functionals, Float );
    ADD_TO_FACTORY( coreFactory, Functionals, Int );
    ADD_TO_FACTORY( coreFactory, Functionals, UInt );
    ADD_TO_FACTORY( coreFactory, Functionals, Color );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector2f );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector2d );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector3f );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector3d );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector4f );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector4d );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector2i );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector2ui );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector3i );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector3ui );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector4i );
    ADD_TO_FACTORY( coreFactory, Functionals, Vector4ui );

    /* --- Functions --- */
    coreFactory->registerNodeCreator<Sources::ScalarBinaryPredicate>(
        Sources::ScalarBinaryPredicate::getTypename() + "_", "Functions" );
    coreFactory->registerNodeCreator<Sources::ScalarUnaryPredicate>(
        Sources::ScalarUnaryPredicate::getTypename() + "_", "Functions" );

    /* --- Graphs --- */
    coreFactory->registerNodeCreator<DataflowGraph>( DataflowGraph::getTypename() + "_", "Graph" );

    registerFactory( coreFactory );
}
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
