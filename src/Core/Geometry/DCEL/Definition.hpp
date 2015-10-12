#ifndef DCEL_FORWARD_DEFINITION_H
#define DCEL_FORWARD_DEFINITION_H

#include <vector>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/**
* Forward declaration of the DCEL classes
*/

class Vertex;
class HalfEdge;
class Edge;
class Face;
class Dcel;

typedef Vertex*   Vertex_ptr;
typedef HalfEdge* HalfEdge_ptr;
typedef Edge*     Edge_ptr;
typedef Face*     Face_ptr;
typedef Dcel*     Dcel_ptr;

typedef std::vector< Vertex_ptr >   VertexList;
typedef std::vector< HalfEdge_ptr > HalfEdgeList;
typedef std::vector< Edge >         EdgeList;
typedef std::vector< Face_ptr >     FaceList;

} // namespace DCEL
} // namespace Core
} // namespace Ra

#endif // DCEL_FORWARD_DEFINITION_H
