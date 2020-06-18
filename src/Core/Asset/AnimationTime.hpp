#ifndef RADIUMENGINE_ANIMATION_TIME_HPP
#define RADIUMENGINE_ANIMATION_TIME_HPP

#include <Core/RaCore.hpp>
#include <algorithm>
#include <limits>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * The AnimationTime class stores the start and end times of an animation.
 */
class RA_CORE_API AnimationTime
{
  public:
    /**
     * Defines the type for time values.
     */
    using Time = Scalar;

    AnimationTime( const Time& start = std::numeric_limits<Time>::max(),
                   const Time& end   = -std::numeric_limits<Time>::max() ) :
        m_start( start ),
        m_end( end ) {}

    AnimationTime( const AnimationTime& time ) = default;

    ~AnimationTime() {}

    ///\name Time
    /// \{

    /**
     * Return the animation start time.
     */
    inline Time getStart() const { return m_start; }

    /**
     * Return the animation start time in \p start.
     */
    inline void setStart( const Time& start ) { m_start = std::min( start, m_end ); }

    /**
     * Return the animation end time.
     */
    inline Time getEnd() const { return m_end; }

    /**
     * Return the animation end time in \p end.
     */
    inline void setEnd( const Time& end ) { m_end = std::max( end, m_start ); }

    /**
     * Extends *this so that \f$ t \in [start,end] \f$.
     */
    inline void extends( const Time& t ) {
        setStart( t );
        setEnd( t );
    }

    /**
     * Performs the merge of *this with \p time, resulting in *this
     * being the smallest AnimationTime containing both *this and \p time.
     */
    inline void extends( const AnimationTime& time ) {
        m_start = std::min( m_start, time.m_start );
        m_end   = std::max( m_end, time.m_end );
    }
    ///\}

    /// \name Query
    /// \{

    /**
     * Return true if \f$ start < end \f$.
     */
    inline bool isValid() const { return m_start < m_end; }

    /**
     * Return the animation duration.
     * \note if the AnimationTime is invalid, returns 0.
     */
    inline Time duration() const { return isValid() ? m_end - m_start : 0; }

    /**
     * Return true if \f$ t \in [start,end] \f$.
     */
    inline bool contains( const Time& t ) const { return ( ( t >= m_start ) && ( t <= m_end ) ); }

    /**
     * Return true if \f$ *this \cap time \neq \emptyset \f$.
     */
    inline bool intersect( const AnimationTime& time ) const {
        return ( contains( time.m_start ) || contains( time.m_end ) || time.contains( m_start ) ||
                 time.contains( m_end ) );
    }
    /// \}

    /// \name Comparison operators
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

    /**
     * Returns true if \f$ t \in [start,end] \f$.
     */
    inline bool operator==( const Time& t ) const { return ( t == m_start ) && ( t == m_end ); }

    /**
     * Returns true if \f$ t < end \f$.
     */
    inline bool operator<( const Time& t ) const { return ( t < m_end ); }

    /**
     * Returns true if \f$ t > start \f$.
     */
    inline bool operator>( const Time& t ) const { return ( t > m_start ); }

  protected:
    /// Animation starting time.
    Time m_start;

    /// Animation ending time.
    Time m_end;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ANIMATION_TIME_HPP
