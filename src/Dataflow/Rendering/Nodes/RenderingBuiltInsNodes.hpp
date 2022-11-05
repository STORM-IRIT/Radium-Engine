#pragma once
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/RaDataflow.hpp>
namespace Ra {
namespace Dataflow {
namespace Rendering {

/**
 * \brief Create the node system default factory for rendering nodes.
 *
 * If needed, the definition of all the rendering nodes can be included using one of the headers
 *  - #include <Dataflow/Rendering/Nodes/ ... xxx ... .hpp>
 *
 *  \return the default resources location for rendering nodes
 */
RA_DATAFLOW_API std::string registerRenderingNodesFactories();
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
