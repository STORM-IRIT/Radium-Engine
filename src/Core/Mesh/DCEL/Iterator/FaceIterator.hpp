#ifndef RADIUMENGINE_DCEL_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
namespace Core {

template < typename OBJECT >
class FIterator : public Iterator< OBJECT > {
public:
    /// CONSTRUCTOR
    FIterator( const Face& f );
    FIterator( const FIterator& it ) = default;

    /// DESTRUCTOR
    ~FIterator();

    /// SIZE
    inline uint size() const override;

    /// RESET
    inline void reset() override;

    /// OPERATOR
    inline FIterator& operator= ( const FIterator& it );
    inline FIterator& operator++();
    inline FIterator& operator--();
    inline bool       operator==( const FIterator& it ) const;

protected:
    /// VARIABLE
    Face_ptr   m_f;
};

//======================================================================================

class FVIterator : public FIterator< Vertex_ptr > {
public:
    /// CONSTRUCTOR
    FVIterator( const Face& f );
    FVIterator( const FVIterator& it ) = default;

    /// DESTRUCTOR
    ~FVIterator();

    /// LIST
    inline VertexList list() const override;

    /// OPERATOR
    inline Vertex_ptr operator->() const override;
};

//======================================================================================

class FHEIterator : public FIterator< HalfEdge_ptr > {
public:
    /// CONSTRUCTOR
    FHEIterator( const Face& f );
    FHEIterator( const FHEIterator& it ) = default;

    /// DESTRUCTOR
    ~FHEIterator();

    /// LIST
    inline HalfEdgeList list() const override;

    /// OPERATOR
    inline HalfEdge_ptr operator->() const override;
};

//======================================================================================

class FFIterator : public FIterator< Face_ptr > {
public:
    /// CONSTRUCTOR
    FFIterator( const Face& f );
    FFIterator( const FFIterator& it ) = default;

    /// DESTRUCTOR
    ~FFIterator();

    /// LIST
    inline FaceList list() const override;

    /// OPERATOR
    inline Face_ptr operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/FaceIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_ITERATOR_HPP
