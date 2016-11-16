#ifndef RADIUMENGINE_DCEL_FACE_FACE_ITERATOR_HPP
#define RADIUMENGINE_DCEL_FACE_FACE_ITERATOR_HPP

#include <Core/Mesh/DCEL/Iterator/Face/FaceIterator.hpp>

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
    inline FaceList list2() const;

    /// OPERATOR
    inline Face* operator->() const override;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/DCEL/Iterator/Face/FFIterator.inl>

#endif // RADIUMENGINE_DCEL_FACE_FACE_ITERATOR_HPP
