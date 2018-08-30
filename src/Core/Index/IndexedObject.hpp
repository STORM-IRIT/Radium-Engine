#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <Core/Index/Index.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

/// An IndexedObject is an object identifiable through it's index.
class DLL_EXPORT IndexedObject {
  public:
    explicit inline IndexedObject( Index idx = Index::Invalid() ) { this->idx = idx; }

    inline IndexedObject( const IndexedObject& id_obj ) { idx = id_obj.idx; }

    virtual inline ~IndexedObject() {}

    /// The index of the object.
    Index idx;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
