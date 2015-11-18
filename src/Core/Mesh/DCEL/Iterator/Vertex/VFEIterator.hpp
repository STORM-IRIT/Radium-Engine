#ifndef RADIUMENGINE_DCEL_VERTEX_FULLEDGE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_FULLEDGE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

namespace Ra {
namespace Core {

class VFEIterator : public VIterator< FullEdge_ptr > {
public:
    /// CONSTRUCTOR
    VFEIterator( const Vertex& v );
    VFEIterator( const VFEIterator& it ) = default;

    /// DESTRUCTOR
    ~VFEIterator();

    /// LIST
    inline FullEdgeList list() const override;

    /// OPERATOR
    inline FullEdge_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VFEIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
