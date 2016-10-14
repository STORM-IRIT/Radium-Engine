#include <Core/Index/IndexMap.hpp>

namespace Ra {
namespace Core {



/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
template < typename T >
IndexMap< T >::IndexMap() :
    m_data(),
    m_index(),
    m_free( 1, Index( 0 ) ) { }



template < typename T >
IndexMap< T >::IndexMap( const IndexMap& id_map ) :
    m_data( id_map.m_data ),
    m_index( id_map.m_index ),
    m_free( id_map.m_free ) { }






/// ===============================================================================
/// DESTRUCTOR
/// ===============================================================================
template < typename T >
IndexMap< T >::~IndexMap() { }






/// ===============================================================================
/// INSERT
/// ===============================================================================
template < typename T >
inline Index IndexMap< T >::insert( const T& obj ) {
    Index idx;
    if( pop_free_index( idx ) ) {
        typename std::deque< Index >::iterator it = std::lower_bound( m_index.begin(), m_index.end(), idx );
        if( it == m_index.end() ) {
            m_data.insert( m_data.end(), obj );
        } else {
            m_data.insert( ( m_data.begin() + ( it - m_index.begin() ) ), obj );
        }
        m_index.insert( it, idx );
    }
    return idx;
}



template < typename T >
inline bool IndexMap< T >::insert( const T& obj,
                                   Index&   idx ) {
    if( !pop_free_index( idx ) ) {
        return false;
    }
    typename std::deque< Index >::iterator it = std::lower_bound( m_index.begin(), m_index.end(), idx );
    if( it == m_index.end() ) {
        m_data.insert( m_data.end(), obj );
    } else {
        m_data.insert( ( m_data.begin() + ( it - m_index.begin() ) ), obj );
    }
    m_index.insert( it, idx );
    return true;
}


template < typename T>
template <typename... Args>
Index IndexMap<T>::emplace(const Args&&... args)
{
    Index idx;
    if( pop_free_index( idx ) ) {
        typename std::deque< Index >::iterator it = std::lower_bound( m_index.begin(), m_index.end(), idx );
        if( it == m_index.end() ) {
            m_data.emplace( m_data.end(), args... );
        } else {
            m_data.emplace( ( m_data.begin() + ( it - m_index.begin() ) ), args... );
        }
        m_index.insert( it, idx );
    }
    return idx;
}

template < typename T>
template<typename... Args >
inline bool IndexMap< T >::emplace(Index&   idx, const Args&&... args ) {
    if( !pop_free_index( idx ) ) {
        return false;
    }
    typename std::deque< Index >::iterator it = std::lower_bound( m_index.begin(), m_index.end(), idx );
    if( it == m_index.end() ) {
        m_data.emplace( m_data.end(), args... );
    } else {
        m_data.emplace( ( m_data.begin() + ( it - m_index.begin() ) ), args... );
    }
    m_index.insert( it, idx );
    return true;
}

/// ===============================================================================
/// REMOVE
/// ===============================================================================
template < typename T >
inline bool IndexMap< T >::remove( const Index& idx ) {
    typename std::deque< Index >::iterator it = std::find( m_index.begin(), m_index.end(), idx );
    if( it == m_index.end() ) {
        return false;
    }
    m_data.erase( m_data.begin() + ( it - m_index.begin() ) );
    m_index.erase( it );
    push_free_index( idx );
    return true;
}



template < typename T >
inline bool IndexMap< T >::remove( const uint i ) {
    if( i >= m_data.size() ) {
        return false;
    }
    push_free_index( m_index[i] );
    m_data.erase( m_data.begin() + i );
    m_index.erase( m_index.begin() + i );
    return true;
}






/// ===============================================================================
/// ACCESS
/// ===============================================================================
template < typename T >
inline const T& IndexMap< T >::at( const Index& idx ) const {
    typename std::deque< Index >::const_iterator it = std::find( m_index.begin(), m_index.end(), idx );
    CORE_ASSERT( it != m_index.end(), "Index not found" );
    return m_data.at( it - m_index.begin() );
}



template < typename T >
inline const T& IndexMap< T >::at( const uint i ) const {
    CORE_ASSERT( ( i < m_data.size() ), "Index i out of bound" );
    return m_data.at( i );
}



template < typename T >
inline bool IndexMap< T >::at( const Index& idx,
                               T&           obj ) const {
    typename std::deque< Index >::const_iterator it = std::find( m_index.begin(), m_index.end(), idx );
    if( it == m_index.end() ) {
        return false;
    }
    obj = m_data.at( it - m_index.begin() );
    return true;
}



template < typename T >
inline bool IndexMap< T >::at( const uint i,
                               T&         obj ) const {
    if( i >= m_data.size() ) {
        return false;
    }
    obj = m_data.at( i );
    return true;
}



template < typename T >
inline T& IndexMap< T >::access( const Index& idx ) {
    typename std::deque< Index >::iterator it = std::find( m_index.begin(), m_index.end(), idx );
    CORE_ASSERT( ( it != m_index.end() ), "Index not found" );
    return m_data[ it - m_index.begin()  ];
}



template < typename T >
inline T& IndexMap< T >::access( const uint i ) {
    CORE_ASSERT( ( i < m_data.size() ) , "Index i out of bound");
    return m_data[i];
}



template < typename T >
inline bool IndexMap< T >::access( const Index& idx,
                                   T&           obj ) {
    if( m_data.empty() ) {
        return false;
    }
    typename std::deque< Index >::iterator it = std::find( m_index.begin(), m_index.end(), idx );
    if( it == m_index.end() ) {
        return false;
    }
    obj = m_data[ it - m_index.begin() ];
    return true;
}



template < typename T >
inline bool IndexMap< T >::access( const uint i,
                                   T&         obj ) {
    if( i >= m_data.size() ) {
        return false;
    }
    obj = m_data[i];
    return true;
}






/// ===============================================================================
/// SIZE
/// ===============================================================================
template < typename T >
inline uint IndexMap< T >::size() const {
    return m_data.size();
}



template < typename T >
inline void IndexMap< T >::clear() {
    m_index.clear();
    m_data.clear();
    m_free.clear();
    m_free.push_back( Index( 0 ) );
}






/// ===============================================================================
/// QUERY
/// ===============================================================================
template < typename T >
inline bool IndexMap< T >::empty() const {
    return  m_data.empty();
}



template < typename T >
inline bool IndexMap< T >::full() const {
    return m_free.empty();
}



template < typename T >
inline bool IndexMap< T >::contain( const Index& idx ) const {
    typename std::deque< Index >::const_iterator it = std::find( m_index.begin(), m_index.end(), idx );
    return !( it == m_index.end() );
}



template < typename T >
inline bool IndexMap< T >::compact() const {
    return ( size_t(m_free.front()) > m_data.size() );
}



template < typename T >
inline Index IndexMap< T >::index( const uint i ) const {
    if( i >= m_index.size() ) {
        return Index::INVALID_IDX();
    }
    return m_index.at( i );
}



template < typename T >
inline bool IndexMap< T >::index( const uint i, Index& idx ) const {
    if( i >= m_index.size() ) {
        return false;
    }
    idx = m_index.at( i );
    return true;
}






/// ===============================================================================
/// OPERATOR
/// ===============================================================================
template < typename T >
inline T& IndexMap< T >::operator[]( const Index& idx ) {
    return access( idx );
}



template < typename T >
inline T& IndexMap< T >::operator[]( const uint i ) {
    return access( i );
}

template < typename T >
inline const T& IndexMap< T >::operator[]( const Index& idx ) const {
    return at( idx );
}



template < typename T >
inline const T& IndexMap< T >::operator[]( const uint i ) const{
    return at( i );
}


template < typename T >
inline Index& IndexMap< T >::operator<<( const T& obj ) {
    return insert( obj );
}



template < typename T >
inline IndexMap< T >& IndexMap< T >::operator>>( const Index& idx ) {
    remove( idx );
    return *this;
}



template < typename T >
inline IndexMap< T >& IndexMap< T >::operator>>( const uint i ) {
    remove( i );
    return *this;
}






/// ===============================================================================
/// INDEX ITERATOR
/// ===============================================================================
template <typename T>
inline typename IndexMap< T >::ConstIndexIterator IndexMap< T >::cbegin_index() const {
    return m_index.cbegin();
}



template <typename T>
inline typename IndexMap< T >::ConstIndexIterator IndexMap< T >::cend_index() const {
    return m_index.cend();
}






/// ===============================================================================
/// DATA ITERATOR
/// ===============================================================================
template <typename T>
inline typename IndexMap< T >::Iterator IndexMap< T >::begin() {
    return m_data.begin();
}



template <typename T>
inline typename IndexMap< T >::Iterator IndexMap< T >::end() {
    return m_data.end();
}


template <typename T>
inline typename IndexMap< T >::ConstIterator IndexMap< T >::begin() const {
    return m_data.begin();
}



template <typename T>
inline typename IndexMap< T >::ConstIterator IndexMap< T >::end() const {
    return m_data.end();
}


template <typename T>
inline typename IndexMap< T >::ConstIterator IndexMap< T >::cbegin() const {
    return m_data.cbegin();
}



template <typename T>
inline typename IndexMap< T >::ConstIterator IndexMap< T >::cend() const {
    return m_data.cend();
}






/// ===============================================================================
/// PUSH
/// ===============================================================================
template < typename T >
inline void IndexMap< T >::push_free_index( const Index& idx ) {
    std::deque<Index>::iterator free_it = std::lower_bound( m_free.begin(), m_free.end(), idx );
    m_free.insert( free_it, idx );
    if( m_data.empty() ) {
        m_free.clear();
        m_free.push_back( Index( 0 ) );
    }
}






/// ===============================================================================
/// POP
/// ===============================================================================
template < typename T >
inline bool IndexMap< T >::pop_free_index( Index& idx ) {
    if( m_free.empty() ) {
        idx = Index::INVALID_IDX();
        return false;
    }
    idx = m_free.front();
    m_free.pop_front();
    if( m_free.empty() ) {
        Index next = idx + 1;
        if( next.isValid() ) {
            if( uint( next.getValue() ) > m_data.size() ) {
                m_free.push_back( next );
            }
        }
    }
    return true;
}




} // namespace Core
} // namespace Ra
