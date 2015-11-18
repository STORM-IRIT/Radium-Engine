#ifndef INDEXMAP_HPP
#define INDEXMAP_HPP

#include <Core/RaCore.hpp>
#include <deque>
#include <algorithm>
#include <assert.h>

#include <Core/Index/Index.hpp>

namespace Ra {
namespace Core {

template <typename T>
class IndexMap {
public:
    /// CONSTRUCTOR
    inline IndexMap();
    inline IndexMap( const IndexMap& id_map );

    /// DESTRUCTOR
    inline ~IndexMap() { }

    /// INSERT
    inline Index insert( const T& obj );
    inline bool  insert( const T& obj, Index& idx );

    /// REMOVE
    inline bool  remove( const Index& idx );
    inline bool  remove( const uint     i );

    /// ACCESS
    inline T     at( const Index& idx ) const;
    inline T     at( const uint     i ) const;
    inline bool  at( const Index& idx, T& obj ) const;
    inline bool  at( const uint     i, T& obj ) const;

    inline T&    access( const Index& idx );
    inline T&    access( const uint     i );
    inline bool  access( const Index& idx, T& obj );
    inline bool  access( const uint     i, T& obj );

    /// SIZE
    inline uint size() const;
    inline void clear();

    /// QUERY
    inline bool  empty() const;
    inline bool  full()  const;
    inline bool  contain( const Index& idx ) const;
    inline bool  compact() const;
    inline Index index( const uint i ) const;

    /// OPERATOR
    inline T&         operator[]( const Index& idx );
    inline T&         operator[]( const uint     i );
    inline Index&     operator<<( const T&     obj );
    inline IndexMap&  operator>>( const Index& idx );

protected:
    /// MAP ENTRY CLASS
    struct IndexMapEntry {
        /// CONSTRUCTOR
        IndexMapEntry( const Index& index = Index::INVALID_IDX() ) : m_idx( index ) { }
        IndexMapEntry( const Index& index, const T& object ) : m_idx( index ), m_obj( object ) { }

        /// VARIABLE
        Index m_idx;
        T     m_obj;

        /// OPERATOR
        bool operator< ( const IndexMapEntry& imp ) const {
            if ( m_idx != imp.m_idx ) {
                return m_idx <  imp.m_idx;
            }
            return false;
        }

        bool operator== ( const IndexMapEntry& imp ) const {
            return ( m_idx == imp.m_idx );
        }
    };

    /// VARIABLE
    std::deque<IndexMapEntry> m_data;

private:
    /// INSERT
    inline void insert_free_index( const Index& idx );

    /// REMOVE
    inline bool remove_free_index( Index& idx );

    /// VARIABLE
    std::deque<Index> m_free;
};

} // namespace Core
} // namespace Ra

#include <Core/Index/IndexMap.inl>

#endif // INDEXMAP_HPP
