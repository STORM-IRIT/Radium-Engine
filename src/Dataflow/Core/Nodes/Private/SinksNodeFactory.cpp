#include <Dataflow/Core/Nodes/Private/FunctionalsNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

namespace Private {

/** TODO : replace this by factory autoregistration at compile time */

#define ADD_SINKS_TO_FACTORY( FACTORY, NAMESPACE, PREFIX ) \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##Sink>( \
        NAMESPACE::PREFIX##Sink::getTypename() + "_", #NAMESPACE );

void registerSinksFactories( NodeFactorySet::mapped_type factory ) {
    /* --- Sinks --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    factory->registerNodeCreator<Sinks::BooleanSink>( Sinks::BooleanSink::getTypename() + "_",
                                                      "Sinks" );

    ADD_SINKS_TO_FACTORY( factory, Sinks, Scalar );
    ADD_SINKS_TO_FACTORY( factory, Sinks, Int );
    ADD_SINKS_TO_FACTORY( factory, Sinks, UInt );
    ADD_SINKS_TO_FACTORY( factory, Sinks, Color );
    ADD_SINKS_TO_FACTORY( factory, Sinks, Vector2 );
    ADD_SINKS_TO_FACTORY( factory, Sinks, Vector3 );
    ADD_SINKS_TO_FACTORY( factory, Sinks, Vector4 );
}
} // namespace Private
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
