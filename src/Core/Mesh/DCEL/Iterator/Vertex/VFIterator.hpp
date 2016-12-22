#ifndef RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>
#include <set>

namespace Ra {
namespace Core {

class VFIterator : public VIterator< Face > {
public:
    /// CONSTRUCTOR
    inline VFIterator( Vertex_ptr& v );
    inline VFIterator( const VFIterator& it ) = default;

    /// DESTRUCTOR
    inline ~VFIterator();

    /// LIST
    //inline FaceList list() const override;
    inline FaceList list() const override;

    /// N-RING
    struct compareFacePtr
    {
        inline bool operator()(const Face_ptr& a1, const Face_ptr& a2) const
        {
            return a1->idx < a2->idx;
        }
    };
    inline void nRing(uint n, std::set<Face_ptr, VFIterator::compareFacePtr>& adjFacesSet);

    /// OPERATOR
    inline Face* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.inl>

#endif // RADIUMENGINE_DCEL_VERTEX_FACE_ITERATOR_HPP
