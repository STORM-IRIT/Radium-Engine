#ifndef RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

template < typename OBJECT >
class VIterator : public Iterator< OBJECT > {
public:
    /// CONSTRUCTOR
    VIterator( const Vertex& v );
    VIterator( const VIterator& it ) = default;

    /// DESTRUCTOR
    ~VIterator();

    /// SIZE
    inline uint size() const override;

    /// RESET
    inline void reset() override;

    /// OPERATOR
    inline VIterator& operator= ( const VIterator& it ) override;
    inline VIterator& operator++() override;
    inline VIterator& operator--() override;
    inline bool       operator==( const VIterator& it ) const override;

protected:
    /// VARIABLE
    Vertex_ptr   m_v;
};

//======================================================================================

class VVIterator : public VIterator< Vertex_ptr > {
public:
    /// CONSTRUCTOR
    VVIterator( const Vertex& v );
    VVIterator( const VVIterator& it ) = default;

    /// DESTRUCTOR
    ~VVIterator();

    /// LIST
    inline VertexList list() const override;

    /// OPERATOR
    inline Vertex_ptr operator->() const override;
};

//======================================================================================

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

//======================================================================================

class VFIterator : public VIterator< Face_ptr > {
public:
    /// CONSTRUCTOR
    VFIterator( const Vertex& v );
    VFIterator( const VFIterator& it ) = default;

    /// DESTRUCTOR
    ~VFIterator();

    /// LIST
    inline FaceList list() const override;

    /// OPERATOR
    inline Face_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/VertexIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_ITERATOR_HPP
