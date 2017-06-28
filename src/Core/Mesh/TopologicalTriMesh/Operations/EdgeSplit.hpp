#include <Core/RaCore.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.h>
#include <Core/Index/IndexMap.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {
namespace TTMOperations {
    void splitEdge( TopologicalMesh& topoMesh, Index edgeIndex, Scalar fraction );
}
}
}
