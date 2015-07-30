#ifndef INDEXMAP_HPP
#define INDEXMAP_HPP

#include <deque>
#include <algorithm>
#include <assert.h>

#include <Core/Index/Index.hpp>

namespace Ra { namespace Core
{

template <typename T>
class IndexMap {
public:
    /// CONSTRUCTOR
    inline RA_API IndexMap();
    inline RA_API IndexMap( const IndexMap& id_map );

    /// DESTRUCTOR
    inline RA_API ~IndexMap() { }

    /// INSERT
    inline RA_API Index insert( const T& obj );
    inline RA_API bool  insert( const T& obj, Index& idx );

    /// REMOVE
    inline RA_API bool  remove( const Index& idx );
    inline RA_API bool  remove( const int      i );

    /// ACCESS
    inline RA_API T     at( const Index& idx ) const;
    inline RA_API T     at( const int      i ) const;
    inline RA_API bool  at( const Index& idx, T& obj ) const;
    inline RA_API bool  at( const int      i, T& obj ) const;

    inline RA_API T&    access( const Index& idx );
    inline RA_API T&    access( const int      i );
    inline RA_API bool  access( const Index& idx, T& obj );
    inline RA_API bool  access( const int      i, T& obj );

    /// SIZE
    inline RA_API uint size() const;
    inline RA_API void clear();

    /// QUERY
    inline RA_API bool  empty() const;
    inline RA_API bool  full()  const;
    inline RA_API bool  contain( const Index& idx ) const;
    inline RA_API Index index( const int i ) const;

    /// OPERATOR
    inline RA_API T&         operator[]( const Index& idx );
    inline RA_API T&         operator[]( const int      i );
    inline RA_API Index&     operator<<( const T&     obj );
    inline RA_API IndexMap&  operator>>( const Index& idx );

protected:
    /// MAP ENTRY CLASS
    struct IndexMapEntry {
        /// CONSTRUCTOR
        IndexMapEntry( const Index& index = Index::INVALID_IDX() ) : m_idx( index ) { }
        IndexMapEntry( const Index& index, const T& object   ) : m_idx( index ), m_obj( object ) { }

        /// VARIABLE
        Index m_idx;
        T     m_obj;

        /// OPERATOR
        bool operator< ( const IndexMapEntry &imp ) const { if( m_idx != imp.m_idx ) return m_idx <  imp.m_idx; return false; }
        bool operator==( const IndexMapEntry &imp ) const { return ( m_idx == imp.m_idx ); }
    };

    /// VARIABLE
    std::deque< IndexMapEntry > m_data;

private:
    /// INSERT
    inline void insert_free_index( const Index& idx );

    /// REMOVE
    inline bool remove_free_index( Index& idx );

    /// VARIABLE
    std::deque< Index > m_free;
};

}} // namespace Ra::Core

#include <Core/Index/IndexMap.inl>

#endif // INDEXMAP_HPP
