#ifndef RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP
#define RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP

#include <vector>
#include <memory>

namespace Ra {
namespace Core {

/**
* Forward declaration of the DCEL classes
*/

class Vertex;
class HalfEdge;
class FullEdge;
class Face;
class Dcel;

typedef std::shared_ptr< Vertex >   Vertex_ptr;
typedef std::shared_ptr< HalfEdge > HalfEdge_ptr;
typedef std::shared_ptr< FullEdge > FullEdge_ptr;
typedef std::shared_ptr< Face >     Face_ptr;
typedef std::shared_ptr< Dcel >     Dcel_ptr;

typedef std::vector< Vertex_ptr >   VertexList;
typedef std::vector< HalfEdge_ptr > HalfEdgeList;
typedef std::vector< FullEdge_ptr > FullEdgeList;
typedef std::vector< Face_ptr >     FaceList;

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP
