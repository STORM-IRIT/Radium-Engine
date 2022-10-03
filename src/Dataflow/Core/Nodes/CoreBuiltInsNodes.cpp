#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Core/NodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

void registerStandardFactories() {
    NodeFactorySet::mapped_type coreFactory { new NodeFactorySet::mapped_type::element_type(
        NodeFactoriesManager::dataFlowBuiltInsFactoryName ) };

    /** TODO : replace this by factory autoregistration at compile time */
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

    /* --- Filters --- */
    coreFactory->registerNodeCreator<Filters::FloatArrayFilter>(
        Filters::FloatArrayFilter::getTypename() + "_", "Filter" );
    coreFactory->registerNodeCreator<Filters::DoubleArrayFilter>(
        Filters::DoubleArrayFilter::getTypename() + "_", "Filter" );
    coreFactory->registerNodeCreator<Filters::IntArrayFilter>(
        Filters::IntArrayFilter::getTypename() + "_", "Filter" );
    coreFactory->registerNodeCreator<Filters::UIntArrayFilter>(
        Filters::UIntArrayFilter::getTypename() + "_", "Filter" );
    coreFactory->registerNodeCreator<Filters::ColorArrayFilter>(
        Filters::ColorArrayFilter::getTypename() + "_", "Filter" );

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
