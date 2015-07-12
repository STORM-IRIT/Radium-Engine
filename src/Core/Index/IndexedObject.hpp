#ifndef RADIUMENGINE_INDEXEDOBJECT_HPP
#define RADIUMENGINE_INDEXEDOBJECT_HPP

#include <index/Index.hpp>

class IndexedObject {
public:
    /// CONSTRUCTOR
    explicit IndexedObject( const Index& idx = Index::INVALID_IDX() ) { this->idx = idx; }
    IndexedObject( const IndexedObject& id_obj ) { idx = id_obj.idx; }

    /// DESTRUCTOR
    ~IndexedObject() { }

    /// VARIABLE
    Index idx;
};

#endif // RADIUMENGINE_INDEXEDOBJECT_HPP
