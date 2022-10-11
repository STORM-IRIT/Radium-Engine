#pragma once

/*
 * Commented out to speedup compilationUser have to include only the desired
 */
#include <Dataflow/Core/Nodes/Functionals/CoreDataFunctionals.hpp>
#include <Dataflow/Core/Nodes/Sinks/CoreDataSinks.hpp>
#include <Dataflow/Core/Nodes/Sources/CoreDataSources.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace NodeFactoriesManager {
void registerStandardFactories();
}
} // namespace Core
} // namespace Dataflow
} // namespace Ra
