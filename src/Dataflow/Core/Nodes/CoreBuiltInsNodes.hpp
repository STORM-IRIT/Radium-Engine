#pragma once

#include <Dataflow/Core/Nodes/Filters/CoreDataFilters.hpp>
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

// These nodes will not be added in the BuiltIns factory as they are templated.
// Only fully specialized template can be added to the factory.
// #include <RadiumAddons/Core/Dataflow/Nodes/FilterNode.hpp>
// #include <RadiumAddons/Core/Dataflow/Nodes/SingleDataSourceNode.hpp>
// #include <RadiumAddons/Core/Dataflow/Nodes/SinkNode.hpp>
