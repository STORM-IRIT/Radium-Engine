#ifndef RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP
#define RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP

#include <memory>
#include <vector>

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

using Vertex_ptr = std::shared_ptr<Vertex>;
using HalfEdge_ptr = std::shared_ptr<HalfEdge>;
using FullEdge_ptr = std::shared_ptr<FullEdge>;
using Face_ptr = std::shared_ptr<Face>;
using Dcel_ptr = std::shared_ptr<Dcel>;

using VertexList = std::vector<Vertex_ptr>;
using HalfEdgeList = std::vector<HalfEdge_ptr>;
using FullEdgeList = std::vector<FullEdge_ptr>;
using FaceList = std::vector<Face_ptr>;

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_FORWARD_DEFINITION_HPP
