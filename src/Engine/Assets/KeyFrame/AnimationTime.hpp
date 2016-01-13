#ifndef RADIUMENGINE_ANIMATION_TIME_HPP
#define RADIUMENGINE_ANIMATION_TIME_HPP

#include <cmath>
#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Asset {

typedef Scalar Time;

class AnimationTime {
public:
    /// CONSTRUCTOR
    AnimationTime( const Time& start = std::numeric_limits< Time >::max(),
                   const Time& end   = std::numeric_limits< Time >::min() ) : m_start( start ), m_end( end ) { }
    AnimationTime( const AnimationTime& time ) = default;

    /// DESTRUCTOR
    ~AnimationTime() { }

    /// TIME
    inline Time getStart() const {
        return m_start;
    }

    inline void setStart( const Time& start ) {
        m_start = start;
    }

    inline Time getEnd() const {
        return m_end;
    }

    inline void setEnd( const Time& end ) {
        m_end = end;
    }

    /// DURATION
    inline Time duration() const {
        return std::abs( m_end - m_start );
    }

    inline void merge( const AnimationTime& time ) {
        m_start = ( m_start > time.m_start ) ? time.m_start : m_start;
        m_end   = ( m_end   < time.m_end   ) ? time.m_end   : m_end;
    }

    /// QUERY
    inline bool contain( const Time& t ) const {
        return ( ( t >= m_start ) && ( t <= m_end ) );
    }

    inline bool intersect( const AnimationTime& time ) const {
        if( isValid() && time.isValid() ) {
            return ( contain( time.m_start ) || contain( time.m_end ) || time.contain( m_start ) || time.contain( m_end ) );
        }
        return false;
    }

    inline bool isValid() const {
        return ( m_start <= m_end );
    }

    /// OPERATOR
    inline bool operator==( const AnimationTime& time ) const { return ( ( m_start == time.m_start ) && ( m_end == time.m_end ) ); }
    inline bool operator!=( const AnimationTime& time ) const { return !( *this == time ); }
    inline bool operator< ( const AnimationTime& time ) const {
        if( ( m_start < time.m_start ) && ( m_end < time.m_start ) ) {
            return true;
        }
        if( m_start == time.m_start ) {
            // bitch please.
            return m_end < time.m_end;
        }
        return false;
    }
    inline bool operator==( const Time& t ) const { return contain( t );    }
    inline bool operator< ( const Time& t ) const { return ( t < m_end   ); }
    inline bool operator> ( const Time& t ) const { return ( t > m_start ); }

protected:
    /// VARIABLE
    Time m_start;
    Time m_end;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ANIMATION_TIME_HPP
