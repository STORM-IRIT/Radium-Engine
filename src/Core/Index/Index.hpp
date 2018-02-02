#ifndef RADIUMENGINE_INDEX_HPP
#define RADIUMENGINE_INDEX_HPP

#include <Core/RaCore.hpp>
#include <limits>

namespace Ra
{
    namespace Core
    {

        class Index
        {
        public:
            /// CONSTRUCTOR
            constexpr Index( const int i = s_invalid );
            constexpr Index( const Index& i );

            /// DESTRUCTOR: Must not be defined, we need it trivial to be
            /// constexpr
            // ~Index() { }

            /// VALID
            constexpr bool isValid() const;

            /// INVALID
            constexpr bool isInvalid() const;
            constexpr void setInvalid();
            static constexpr const Index Invalid()
            {
                return Index( s_invalid );
            }
            static constexpr Index Max()
            {
                return Index( s_maxIdx );
            }

            /// INDEX
            constexpr int  getValue() const;
            constexpr void setValue( const int i );

            /// OPERATOR
            constexpr Index& operator= ( const Index& id );
            constexpr Index& operator++();
            constexpr Index& operator--();
            explicit operator size_t() const { return size_t( m_idx ); }
            constexpr operator int() const
            {
                return m_idx;
            }
            friend constexpr Index operator+ ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return Invalid();
                }
                return Index( id0.m_idx + id1.m_idx );
            }
            friend constexpr Index operator+ ( const Index& id,  const int    off )
            {
                return Index ( ( id.m_idx < -off ) ? s_invalid
                                                   : ( id.m_idx + off ) );
            }
            friend constexpr Index operator- ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return Invalid();
                }
                return Index( id0.m_idx - id1.m_idx );
            }
            friend constexpr Index operator- ( const Index& id,  const int    off )
            {
                return id + (-off);
            }
            friend constexpr bool  operator== ( const Index& id0, const Index& id1 )
            {
                return ( id0.m_idx == id1.m_idx );
            }
            friend constexpr bool  operator!= ( const Index& id0, const Index& id1 )
            {
                return ( id0.m_idx != id1.m_idx );
            }
            friend constexpr bool  operator< ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx <  id1.m_idx );
            }
            friend constexpr bool  operator<= ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx <= id1.m_idx );
            }
            friend constexpr bool  operator> ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx >  id1.m_idx );
            }
            friend constexpr bool  operator>= ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx >= id1.m_idx );
            }
            friend constexpr bool  operator== ( const Index& id,  const int    i )
            {
                return ( id.m_idx == i );
            }
            friend constexpr bool  operator!= ( const Index& id,  const int    i )
            {
                return ( id.m_idx != i );
            }
            friend constexpr bool  operator< ( const Index& id,  const int    i )
            {
                return ( id.m_idx <  i );
            }
            friend constexpr bool  operator<= ( const Index& id,  const int    i )
            {
                return ( id.m_idx <= i );
            }
            friend constexpr bool  operator> ( const Index& id,  const int    i )
            {
                return ( id.m_idx >  i );
            }
            friend constexpr bool  operator>= ( const Index& id,  const int    i )
            {
                return ( id.m_idx >= i );
            }
            friend constexpr bool  operator== ( const int    i, const Index& id )
            {
                return ( id.m_idx == i );
            }
            friend constexpr bool  operator!= ( const int    i, const Index& id )
            {
                return ( id.m_idx != i );
            }
            friend constexpr bool  operator< ( const int    i, const Index& id )
            {
                return ( i < id.m_idx );
            }
            friend constexpr bool  operator<= ( const int    i, const Index& id )
            {
                return ( i <= id.m_idx );
            }
            friend constexpr bool  operator> ( const int    i, const Index& id )
            {
                return ( i > id.m_idx );
            }
            friend constexpr bool  operator>= ( const int    i, const Index& id )
            {
                return ( i >= id.m_idx );
            }

        protected:
            /// VARIABLE
            int m_idx;

        private:
            /// CONSTANT
            static const int s_invalid = -1;
            static const int s_maxIdx = std::numeric_limits<int>::max();
        };

    }
} // namespace Ra::Core

#include <Core/Index/Index.inl>

#endif // RADIUMENGINE_INDEX_HPP
