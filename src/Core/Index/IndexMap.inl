namespace Ra
{
    namespace Core
    {

        /// CONSTRUCTOR
        template <typename T>
        IndexMap<T>::IndexMap()
        {
            m_data.clear();
            m_free.push_back( Index( 0 ) );
        }

        template <typename T>
        IndexMap<T>::IndexMap( const IndexMap& id_map )
        {
            m_data = id_map.m_data;
            m_free = id_map.m_free;
        }

        /// INSERT
        template <typename T>
        inline Index IndexMap<T>::insert( const T& obj )
        {
            Index idx;
            if ( remove_free_index( idx ) )
            {
                IndexMapEntry imp( idx, obj );
                typename std::deque<IndexMapEntry>::iterator it = std::lower_bound( m_data.begin(), m_data.end(), imp );
                m_data.insert( it, imp );
            }
            return idx;
        }

        template <typename T>
        inline bool IndexMap<T>::insert( const T& obj, Index& idx )
        {
            if ( !remove_free_index( idx ) )
            {
                return false;
            }
            IndexMapEntry imp( idx, obj );
            typename std::deque<IndexMapEntry>::iterator it = std::lower_bound( m_data.begin(), m_data.end(), imp );
            m_data.insert( it, imp );
            return true;
        }

        /// REMOVE
        template <typename T>
        inline bool IndexMap<T>::remove( const Index& idx )
        {
            if ( m_data.empty() )
            {
                return false;
            }
            IndexMapEntry imp( idx );
            typename std::deque<IndexMapEntry>::iterator data_it = std::find( m_data.begin(), m_data.end(), imp );
            if ( data_it == m_data.end() )
            {
                return false;
            }
            m_data.erase( data_it );
            insert_free_index( idx );
            return true;
        }

        template <typename T>
        inline bool IndexMap<T>::remove( const int i )
        {
            if ( ( i < 0 ) || ( i >= m_data.size() ) || m_data.empty() )
            {
                return false;
            }
            IndexMapEntry imp( m_data[i].idx );
            typename std::deque<IndexMapEntry>::iterator data_it = std::find( m_data.begin(), m_data.end(), imp );
            insert_free_index( m_data[i].idx );
            m_data.erase( data_it );
            return true;
        }

        /// ACCESS
        template <typename T>
        inline T IndexMap<T>::at( const Index& idx ) const
        {
            IndexMapEntry imp( idx );
            typename std::deque<IndexMapEntry>::const_iterator it = std::find( m_data.begin(), m_data.end(), imp );
            CORE_ASSERT( it != m_data.end(), "Index not found" );
            return it->m_obj;
        }

        template <typename T>
        inline T IndexMap<T>::at( const int i ) const
        {
            CORE_ASSERT( ( i >= 0 ) || ( uint( i ) < m_data.size() ) || m_data.empty(), "Invalid Index" );
            return m_data.at( i ).m_obj;
        }

        template <typename T>
        inline bool IndexMap<T>::at( const Index& idx, T& obj ) const
        {
            if ( m_data.empty() )
            {
                return false;
            }
            IndexMapEntry imp( idx );
            typename std::deque<IndexMapEntry>::const_iterator it = std::find( m_data.begin(), m_data.end(), imp );
            if ( it == m_data.end() )
            {
                return false;
            }
            obj = it->m_obj;
            return true;
        }

        template <typename T>
        inline bool IndexMap<T>::at( const int i, T& obj ) const
        {
            if ( ( i < 0 ) || ( i >= m_data.size() ) || m_data.empty() )
            {
                return false;
            }
            obj = m_data.at( i ).m_obj;
            return true;
        }

        template <typename T>
        inline T& IndexMap<T>::access( const Index& idx )
        {
            IndexMapEntry imp( idx );
            typename std::deque<IndexMapEntry>::iterator it = std::find( m_data.begin(), m_data.end(), imp );
            assert( it != m_data.end() );
            return it->m_obj;
        }

        template <typename T>
        inline T& IndexMap<T>::access( const int i )
        {
            assert( ( i >= 0 ) || ( uint( i ) < m_data.size() ) || m_data.empty() );
            return m_data[i].m_obj;
        }

        template <typename T>
        inline bool IndexMap<T>::access( const Index& idx, T& obj )
        {
            if ( m_data.empty() )
            {
                return false;
            }
            IndexMapEntry imp( idx );
            typename std::deque<IndexMapEntry>::iterator it = std::find( m_data.begin(), m_data.end(), imp );
            if ( it == m_data.end() )
            {
                return false;
            }
            obj = it->m_obj;
            return true;
        }

        template <typename T>
        inline bool IndexMap<T>::access( const int i, T& obj )
        {
            if ( ( i < 0 ) || ( i >= m_data.size() ) || m_data.empty() )
            {
                return false;
            }
            obj = m_data[i].m_obj;
            return true;
        }

        /// SIZE
        template <typename T>
        inline uint IndexMap<T>::size() const
        {
            return m_data.size();
        }

        template <typename T>
        inline void IndexMap<T>::clear()
        {
            m_data.clear();
            m_free.clear();
            m_free.push_back( Index( 0 ) );
        }

        /// QUERY
        template <typename T>
        inline bool IndexMap<T>::empty() const
        {
            return  m_data.empty();
        }

        template <typename T>
        inline bool IndexMap<T>::full()  const
        {
            return m_free.empty();
        }

        template <typename T>
        inline bool IndexMap<T>::contain( const Index& idx ) const
        {
            IndexMapEntry imp( idx );
            typename std::deque<IndexMapEntry>::const_iterator it = std::find( m_data.begin(), m_data.end(), imp );
            if ( it == m_data.end() )
            {
                return false;
            }
            return true;
        }

        template <typename T>
        inline Index IndexMap<T>::index( const int i ) const
        {
            if ( ( i < 0 ) || ( i >= m_data.size() ) || m_data.empty() )
            {
                return Index::INVALID_IDX();
            }
            return m_data[i].m_idx;
        }

        /// OPERATOR
        template <typename T>
        inline T& IndexMap<T>::operator[]( const Index& idx )
        {
            return access( idx );
        }

        template <typename T>
        inline T& IndexMap<T>::operator[]( const int i )
        {
            return access( i );
        }

        template <typename T>
        inline Index& IndexMap<T>::operator<< ( const T& obj )
        {
            return insert( obj );
        }

        template <typename T>
        inline IndexMap<T>& IndexMap<T>::operator>> ( const Index& idx )
        {
            remove( idx );
            return *this;
        }

        /// INSERT
        template <typename T>
        inline void IndexMap<T>::insert_free_index( const Index& idx )
        {
            std::deque<Index>::iterator free_it = std::lower_bound( m_free.begin(), m_free.end(), idx );
            m_free.insert( free_it, idx );

            if ( m_data.empty() )
            {
                m_free.clear();
                m_free.push_back( Index( 0 ) );
            }
        }

        /// REMOVE
        template <typename T>
        inline bool IndexMap<T>::remove_free_index( Index& idx )
        {
            if ( m_free.empty() )
            {
                idx = Index::INVALID_IDX();
                return false;
            }
            idx = m_free.front();
            m_free.pop_front();
            if ( m_free.empty() )
            {
                Index next = idx + 1;
                if ( next.isValid() )
                {
                    if ( uint( next.getValue() ) > m_data.size() )
                    {
                        m_free.push_back( next );
                    }
                }
            }
            return true;
        }

    }
} // namespace Ra::Core
