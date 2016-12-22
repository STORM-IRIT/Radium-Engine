#ifndef RADIUMENGINE_DCEL_FACE_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>
#include <set>

namespace Ra {
namespace Core {

class FFIterator : public FIterator< Face > {
public:
    /// CONSTRUCTOR
    FFIterator( Face_ptr& f );
    FFIterator( const FFIterator& it ) = default;

    /// DESTRUCTOR
    ~FFIterator();

    /// LIST
    inline FaceList list() const override;

    /// N-RING
    struct compareFacePtr
    {
        inline bool operator()(const Face_ptr& a1, const Face_ptr& a2) const
        {
            return a1->idx < a2->idx;
        }
    };
    inline void nRing(uint n, std::set<Face_ptr, FFIterator::compareFacePtr>& adjFacesSet);


    /// OPERATOR
    inline Face* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FFIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_FACE_ITERATOR_HPP
