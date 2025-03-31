#include <Dataflow/Core/Nodes/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/Nodes/Private/FunctionalsNodeFactory.hpp>
#include <memory>
#include <string>

namespace Ra {
namespace Dataflow {
namespace Core {

namespace NodeFactoriesManager {

namespace Private {

/** TODO : replace this by factory autoregistration at compile time */
#define ADD_FUNCTIONALS_TO_FACTORY( FACTORY, NAMESPACE, SUFFIX )               \
    FACTORY->registerNodeCreator<NAMESPACE::BinaryOp##SUFFIX>(                 \
        NAMESPACE::BinaryOp##SUFFIX::getTypename() + "_", #NAMESPACE );        \
    FACTORY->registerNodeCreator<NAMESPACE::BinaryPredicate##SUFFIX>(          \
        NAMESPACE::BinaryPredicate##SUFFIX::getTypename() + "_", #NAMESPACE ); \
    FACTORY->registerNodeCreator<NAMESPACE::Transform##SUFFIX>(                \
        NAMESPACE::Transform##SUFFIX::getTypename() + "_", #NAMESPACE );

void registerFunctionalsFactories( NodeFactorySet::mapped_type factory ) {
    /* --- Functionals */
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, Scalar );
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, Int );
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, UInt );
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, Color );
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, Vector2 );
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, Vector3 );
    ADD_FUNCTIONALS_TO_FACTORY( factory, Functionals, Vector4 );
}
} // namespace Private
} // namespace NodeFactoriesManager
} // namespace Core
} // namespace Dataflow
} // namespace Ra
