#ifndef RADIUMENGINE_INDEX_HPP
#define RADIUMENGINE_INDEX_HPP

#include <limits>

#include <Core/CoreMacros.hpp>

namespace Ra
{
    namespace Core
    {

        class RA_API Index
        {
        public:
            /// CONSTRUCTOR
            Index( const int i = s_invalid );
            Index( const Index& i );

            /// DESTRUCTOR
            ~Index() { }

            /// COPY
            inline void copy( const Index& id );

            /// VALID
            inline bool isValid() const;

            /// INVALID
            inline bool isInvalid() const;
            inline void setInvalid();
            static Index INVALID_IDX()
            {
                return Index( s_invalid );
            }
            static Index MAX_IDX()
            {
                return Index( s_maxIdx );
            }

            /// INDEX
            inline int  getValue() const;
            inline void setValue( const int i );

            /// OPERATOR
            inline Index& operator= ( const Index& id );
            inline Index& operator++();
            inline Index& operator--();
            inline operator int()
            {
                return m_idx;
            }
            friend inline Index operator+ ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return INVALID_IDX();
                }
                return Index( id0.m_idx + id1.m_idx );
            }
            friend inline Index operator+ ( const Index& id,  const int    off )
            {
                int i;
                i = ( ( i = id.m_idx + off ) < 0 ) ? s_invalid : i;
                return Index( i );
            }
            friend inline Index operator- ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return INVALID_IDX();
                }
                return Index( id0.m_idx - id1.m_idx );
            }
            friend inline Index operator- ( const Index& id,  const int    off )
            {
                int i;
                i = ( ( i = id.m_idx - off ) < 0 ) ? s_invalid : i;
                return Index( i );
            }
            friend inline bool  operator== ( const Index& id0, const Index& id1 )
            {
                return ( id0.m_idx == id1.m_idx );
            }
            friend inline bool  operator!= ( const Index& id0, const Index& id1 )
            {
                return ( id0.m_idx != id1.m_idx );
            }
            friend inline bool  operator< ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx <  id1.m_idx );
            }
            friend inline bool  operator<= ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx <= id1.m_idx );
            }
            friend inline bool  operator> ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx >  id1.m_idx );
            }
            friend inline bool  operator>= ( const Index& id0, const Index& id1 )
            {
                if ( id0.isInvalid() || id1.isInvalid() )
                {
                    return false;
                }
                return ( id0.m_idx >= id1.m_idx );
            }
            friend inline bool  operator== ( const Index& id,  const int    i )
            {
                return ( id.m_idx == i );
            }
            friend inline bool  operator!= ( const Index& id,  const int    i )
            {
                return ( id.m_idx != i );
            }
            friend inline bool  operator< ( const Index& id,  const int    i )
            {
                return ( id.m_idx <  i );
            }
            friend inline bool  operator<= ( const Index& id,  const int    i )
            {
                return ( id.m_idx <= i );
            }
            friend inline bool  operator> ( const Index& id,  const int    i )
            {
                return ( id.m_idx >  i );
            }
            friend inline bool  operator>= ( const Index& id,  const int    i )
            {
                return ( id.m_idx >= i );
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
