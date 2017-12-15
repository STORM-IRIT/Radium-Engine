#ifndef RADIUMENGINE_DCEL_VERTEX_HALFEDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_HALFEDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class [[deprecated]] VHEIterator : public VIterator< HalfEdge > {
public:
    /// CONSTRUCTOR
    VHEIterator( Vertex_ptr& v );
    VHEIterator( const VHEIterator& it ) = default;

    /// DESTRUCTOR
    ~VHEIterator();

    /// LIST
    inline HalfEdgeList list() const override;

    /// OPERATOR
    inline HalfEdge* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_HALFEDGE_ITERATOR_HPP
