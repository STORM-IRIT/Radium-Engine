#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/GraphNodes.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/Nodes/CoreBuiltInsNodes.hpp>

void fCoreNodes__Initializer();

struct fCoreNodes__Initializer_t_ {
    fCoreNodes__Initializer_t_() { ::fCoreNodes__Initializer(); }
};
static fCoreNodes__Initializer_t_ fCoreNodes__Initializer__;
using namespace Ra::Dataflow::Core;
void fCoreNodes__Initializer() {
    NodeFactoriesManager::registerStandardFactories();
    PortFactory::createInstance();
}
