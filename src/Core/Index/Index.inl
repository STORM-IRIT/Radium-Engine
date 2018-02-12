namespace Ra
{
    namespace Core
    {
        /// CONSTRUCTOR
        constexpr Index::Index( int i )
            : m_idx ( ( i < 0 ) ? s_invalid : i ) { }

        constexpr Index::Index( const Index& i )
            : m_idx ( i.m_idx ) { }

        /// VALID
        constexpr bool  Index::isValid() const
        {
            return ( m_idx != s_invalid );
        }

        /// INVALID
        constexpr bool  Index::isInvalid() const
        {
            return ( m_idx < 0 );
        }
        constexpr void  Index::setInvalid()
        {
            m_idx = s_invalid;
        }
        constexpr Index Index::Invalid()
        {
            return Index( s_invalid );
        }
        constexpr Index Index::Max()
        {
            return Index( s_maxIdx );
        }

        /// INDEX
        constexpr int  Index::getValue() const
        {
            return m_idx;
        }
        constexpr void Index::setValue( const int i )
        {
            m_idx = ( i < 0 ) ? s_invalid : i;
        }

        /// CAST
        constexpr Index::operator int() const
        {
            return m_idx;
        }

        /// OPERATOR
        constexpr Index& Index::operator= ( const Index& id )
        {
            m_idx = id.m_idx;
            return *this;
        }
        constexpr Index& Index::operator++()
        {
            m_idx++;
            return *this;
        }
        constexpr Index& Index::operator--()
        {
            if ( m_idx != s_invalid )
            {
                m_idx--;
            }
            return *this;
        }

        constexpr Index Index::operator+ ( const Index& id )
        {
            if ( isInvalid() || id.isInvalid() )
            {
                return Index::Invalid();
            }
            return Index( m_idx + id.m_idx );
        }
        template <typename Integer>
        constexpr Index Index::operator+ ( const Integer& id )
        {
            return (*this) + Index(id);
        }

        constexpr Index Index::operator- ( const Index& id )
        {
            if ( isInvalid() || id.isInvalid() )
            {
                return Index::Invalid();
            }
            return Index( m_idx - id.m_idx );
        }
        template <typename Integer>
        constexpr Index Index::operator- ( const Integer& id )
        {
            return (*this) - Index(id);
        }

        constexpr bool Index::operator== ( const Index& id )
        {
            return ( m_idx == id.m_idx );
        }
        template <typename Integer>
        constexpr bool Index::operator== ( const Integer& i )
        {
            return ( m_idx == i );
        }

        constexpr bool Index::operator!= ( const Index& id )
        {
            return ( m_idx != id.m_idx );
        }
        template <typename Integer>
        constexpr bool Index::operator!= ( const Integer& i )
        {
            return ( m_idx != i );
        }

        constexpr bool Index::operator< ( const Index& id )
        {
            if ( isInvalid() || id.isInvalid() )
            {
                return false;
            }
            return ( m_idx <  id.m_idx );
        }
        template <typename Integer>
        constexpr bool Index::operator< ( const Integer& i )
        {
            return ( m_idx <  i );
        }

        constexpr bool Index::operator<= ( const Index& id )
        {
            if ( isInvalid() || id.isInvalid() )
            {
                return false;
            }
            return ( m_idx <= id.m_idx );
        }
        template <typename Integer>
        constexpr bool Index::operator<= ( const Integer& i )
        {
            return ( m_idx <= i );
        }

        constexpr bool Index::operator> ( const Index& id )
        {
            if ( isInvalid() || id.isInvalid() )
            {
                return false;
            }
            return ( m_idx >  id.m_idx );
        }
        template <typename Integer>
        constexpr bool Index::operator> ( const Integer& i )
        {
            return ( m_idx >  i );
        }

        constexpr bool Index::operator>= ( const Index& id )
        {
            if ( isInvalid() || id.isInvalid() )
            {
                return false;
            }
            return ( m_idx >= id.m_idx );
        }
        template <typename Integer>
        constexpr bool Index::operator>= ( const Integer& i )
        {
            return ( m_idx >= i );
        }

    } // namespace Ra::Core
} // namespace Ra
