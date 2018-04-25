#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <Core/Container/Index.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Container {

class DLL_EXPORT IndexedObject {
  public:
    /// CONSTRUCTOR
    explicit inline IndexedObject( Index idx = Index::Invalid() ) { this->idx = idx; }
    inline IndexedObject( const IndexedObject& id_obj ) { idx = id_obj.idx; }

    /// DESTRUCTOR
    virtual inline ~IndexedObject() {}

    /// VARIABLE
    Index idx;
};

} // namespace Container
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
