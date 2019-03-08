#ifndef RADIUMENGINE_ANIMATION_TIME_HPP
#define RADIUMENGINE_ANIMATION_TIME_HPP

#include <Core/CoreMacros.hpp>
#include <cmath>
#include <limits>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * Defines the type for time values.
 */
using Time = Scalar;

/**
 * The AnimationTime class stores the start and end times of an animation.
 */
class AnimationTime {
  public:
    AnimationTime( const Time& start = std::numeric_limits<Time>::max(),
                   const Time& end = std::numeric_limits<Time>::min() ) :
        m_start( start ),
        m_end( end ) {}
    AnimationTime( const AnimationTime& time ) = default;

    ~AnimationTime() {}

    /// \name Time
    /// \{

    /**
     * Return the animation start time.
     */
    inline Time getStart() const { return m_start; }

    /**
     * Return the animation start time in \p start.
     */
    inline void setStart( const Time& start ) { m_start = start; }

    /**
     * Return the animation end time.
     */
    inline Time getEnd() const { return m_end; }

    /**
     * Return the animation end time in \p end.
     */
    inline void setEnd( const Time& end ) { m_end = end; }
    /// \}

    /// \name Duration
    /// \{

    /**
     * Return the animation duration.
     */
    inline Time duration() const { return std::abs( m_end - m_start ); }

    /**
     * Performs the merge of *this with \p time, resulting in *this
     * being the smallest AnimationTime containing both *this and \p time.
     */
    inline void merge( const AnimationTime& time ) {
        m_start = ( m_start > time.m_start ) ? time.m_start : m_start;
        m_end = ( m_end < time.m_end ) ? time.m_end : m_end;
    }
    ///\}

    /// \name Query
    /// \{

    /**
     * Return true if \f$ t \in [start,end] \f$.
     */
    inline bool contain( const Time& t ) const { return ( ( t >= m_start ) && ( t <= m_end ) ); }

    /**
     * Return true if \f$ *this \cap time \neq \emptyset \f$.
     */
    inline bool intersect( const AnimationTime& time ) const {
        if ( isValid() && time.isValid() )
        {
            return ( contain( time.m_start ) || contain( time.m_end ) || time.contain( m_start ) ||
                     time.contain( m_end ) );
        }
        return false;
    }

    /**
     * Returns true if \f$ start < end \f$.
     */
    inline bool isValid() const { return ( m_start <= m_end ); }
    /// \}

    /// \name Arithmetic operators
    /// \{

    /**
     * Returns true if *this and \p time represent the same animation time.
     */
    inline bool operator==( const AnimationTime& time ) const {
        return ( ( m_start == time.m_start ) && ( m_end == time.m_end ) );
    }

    /**
     * Returns true if *this and \p time represent different animation times.
     */
    inline bool operator!=( const AnimationTime& time ) const { return !( *this == time ); }

    /** Returns true is *this starts before \p time or if they both start at
     * the same moment but this ends before \p time.
     */
    inline bool operator<( const AnimationTime& time ) const {
        if ( ( m_start < time.m_start ) && ( m_end < time.m_start ) )
        {
            return true;
        }
        if ( m_start == time.m_start )
        {
            // bitch please.
            return m_end < time.m_end;
        }
        return false;
    }

    /**
     * Returns true if \f$ t \in [start,end] \f$.
     */
    inline bool operator==( const Time& t ) const { return contain( t ); }

    /**
     * Returns true if \f$ t < end \f$.
     */
    inline bool operator<( const Time& t ) const { return ( t < m_end ); }

    /**
     * Returns true if \f$ t > start \f$.
     */
    inline bool operator>( const Time& t ) const { return ( t > m_start ); }
    /// \}

  protected:
    /// Animation starting time.
    Time m_start;

    /// Animation ending time.
    Time m_end;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ANIMATION_TIME_HPP
