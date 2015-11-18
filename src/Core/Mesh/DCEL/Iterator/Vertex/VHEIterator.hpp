#ifndef RADIUMENGINE_DCEL_VERTEX_HALFEDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_HALFEDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class VHEIterator : public VIterator< HalfEdge_ptr > {
public:
    /// CONSTRUCTOR
    VHEIterator( const Vertex& v );
    VHEIterator( const VHEIterator& it ) = default;

    /// DESTRUCTOR
    ~VHEIterator();

    /// LIST
    inline HalfEdgeList list() const override;

    /// OPERATOR
    inline HalfEdge_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_HALFEDGE_ITERATOR_HPP
