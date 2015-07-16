namespace Ra { namespace Core
{

/// COPY
inline void Index::copy( const Index& id ) { m_idx = id.m_idx; }

/// VALID
inline bool  Index::isValid() const { return ( m_idx != s_invalid ); }

/// INVALID
inline bool  Index::isInvalid() const { return ( m_idx == s_invalid ); }
inline void  Index::setInvalid()      { m_idx = s_invalid;             }

/// INDEX
inline int  Index::getValue() const        { return m_idx; }
inline void Index::setValue( const int i ) { m_idx = ( i < 0 ) ? s_invalid : i; }

/// OPERATOR
       inline Index& Index::operator= ( const Index& id ) { m_idx = id.m_idx; return *this; }
       inline Index& Index::operator++() { m_idx++; return *this; }
       inline Index& Index::operator--() { if( m_idx != s_invalid ) m_idx--; return *this; }

}} // namespace Ra::Core
