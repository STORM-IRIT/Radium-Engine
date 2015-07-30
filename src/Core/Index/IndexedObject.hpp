#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <Core/Index/Index.hpp>

namespace Ra { namespace Core
{

class RA_API IndexedObject {
public:
    /// CONSTRUCTOR
    explicit IndexedObject( const Index& idx = Index::INVALID_IDX() ) { this->idx = idx; }
    IndexedObject( const IndexedObject& id_obj ) { idx = id_obj.idx; }

    /// DESTRUCTOR
    ~IndexedObject() { }

    /// VARIABLE
    Index idx;
};

}} // namespace Ra::Core

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
