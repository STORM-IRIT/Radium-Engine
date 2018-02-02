namespace Ra
{
    namespace Core
    {
        /// CONSTRUCTOR
        constexpr Index::Index( const int i )
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
            return ( m_idx == s_invalid );
        }
        constexpr void  Index::setInvalid()
        {
            m_idx = s_invalid;
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

    }
} // namespace Ra::Core
