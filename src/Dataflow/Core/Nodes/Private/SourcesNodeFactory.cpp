#include <Dataflow/Core/Nodes/Private/SourcesNodeFactory.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

namespace Private {

/** TODO : replace this by factory autoregistration at compile time */
#define ADD_SOURCES_TO_FACTORY( FACTORY, NAMESPACE, PREFIX ) \
    FACTORY->registerNodeCreator<NAMESPACE::PREFIX##Source>( \
        NAMESPACE::PREFIX##Source::getTypename() + "_", #NAMESPACE )

void registerSourcesFactories( NodeFactorySet::mapped_type factory ) {
    /* --- Sources --- */
    // bool could not be declared as others, because of the specificity of std::vector<bool> that is
    // not compatible with Ra::Core::VectorArray implementation see
    // https://en.cppreference.com/w/cpp/container/vector_bool Right now, there is no
    // Ra::Core::VectorArray of bool
    factory->registerNodeCreator<Sources::BooleanSource>(
        Sources::BooleanSource::getTypename() + "_", "Sources" );
    // prevent Scalar type collision with float or double in factory
    ADD_SOURCES_TO_FACTORY( factory, Sources, Scalar );
    ADD_SOURCES_TO_FACTORY( factory, Sources, Int );
    ADD_SOURCES_TO_FACTORY( factory, Sources, UInt );
    ADD_SOURCES_TO_FACTORY( factory, Sources, Color );
    ADD_SOURCES_TO_FACTORY( factory, Sources, Vector2 );
    ADD_SOURCES_TO_FACTORY( factory, Sources, Vector3 );
    ADD_SOURCES_TO_FACTORY( factory, Sources, Vector4 );
}
} // namespace Private
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
