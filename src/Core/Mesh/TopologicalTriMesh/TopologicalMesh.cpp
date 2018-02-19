#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra{
namespace Core{

Scalar dot(TopoVector3 a, TopoVector3 b){
    return a.dot(b);

}
TopoVector3 cross(TopoVector3 a, TopoVector3 b){
    return a.cross(b);
}
}
}
