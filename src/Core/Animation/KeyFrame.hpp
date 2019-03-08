#ifndef RADIUMENGINE_KEY_FRAME_HPP
#define RADIUMENGINE_KEY_FRAME_HPP

#include <map>
#include <set>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Core/Math/Types.hpp>

#include <Core/Animation/AnimationTime.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * The KeyFrame class is a generic container of FRAME values.
 * The FRAME values are bound to a certain time within the AnimationTime.
 */
template <class FRAME>
class KeyFrame {
  public:
    KeyFrame( const AnimationTime& time = AnimationTime() ) : m_time( time ) {}

    KeyFrame( const KeyFrame& keyframe ) = default;

    inline KeyFrame& operator=( const KeyFrame& keyframe ) {
        m_time = keyframe.m_time;
        m_keyframe = keyframe.m_keyframe;
        return *this;
    }

    virtual ~KeyFrame() {}

    /// \name Time
    /// \{

    /**
     * Return the AnimationTime for the KeyFrame.
     */
    inline AnimationTime getAnimationTime() const { return m_time; }

    /**
     * Sets the AnimationTime for the KeyFrame.
     * \note No check is done regarding already registered FRAME values.
     */
    // Is this useful? Can this create problems?
    inline void setAnimationTime( const AnimationTime& time ) { m_time = time; }

    /**
     * Return the list of instants to which a FRAME is bound.
     */
    inline std::vector<Time> timeSchedule() const {
        std::vector<Time> time;
        for ( const auto& it : m_keyframe )
        {
            const Time t = Scalar( it.first );
            time.push_back( t );
        }
        return time;
    }
    /// \}

    /// \name Management
    /// \{

    /**
     * Return the \p i-th FRAME, provided it exists.
     */
    inline FRAME getKeyFrame( const uint i ) const {
        CORE_ASSERT( ( i < size() ), "Index i out of bound" );
        return ( m_keyframe.begin() + i )->second;
    }

    /**
     * Return the \p i-th FRAME, provided it exists.
     */
    inline FRAME& getKeyFrame( const uint i ) {
        CORE_ASSERT( ( i < size() ), "Index i out of bound" );
        return ( m_keyframe.begin() + i )->second;
    }

    /**
     * Return the FRAME coresponding to time \p t.
     * \note If \f$ t \notin AnimationTime \f$, then the default FRAME value
     *       is returned.
     *       The closest FRAMEs are linearly interpolated otherwise.
     */
    inline FRAME at( const Time& t ) const {
        if ( !m_time.contain( t ) )
        {
            return defaultFrame();
        }
        FRAME F0;
        FRAME F1;
        Scalar dt;
        findRange( t, F0, F1, dt );
        return interpolate( F0, F1, dt );
    }

    /**
     * Replace the \p i-th FRAME by \p frame, provided it exists.
     */
    inline void setKeyFrame( const uint i, const FRAME& frame ) {
        CORE_ASSERT( ( i < size() ), "Index i out of bound" );
        Time t = ( m_keyframe.begin() + i )->first;
        insertKeyFrame( t, frame );
    }

    /**
     * Register \p frame as the FRAME at instant \p t.
     * \note If \f$ t \notin AnimationTime \f$, then the AnimationTime is
     *       extended to include \p t.
     */
    inline void insertKeyFrame( const Time& t, const FRAME& frame ) {
        if ( t < m_time.getStart() )
        {
            m_time.setStart( t );
        }
        if ( t > m_time.getEnd() )
        {
            m_time.setEnd( t );
        }
        m_keyframe[t] = frame;
    }

    /**
     * Remove all FRAMEs, but keeps the AnimationTime.
     */
    inline void clear() { m_keyframe.clear(); }

    /**
     * Remove all FRAMEs and clears the AnimationTime.
     */
    inline void reset() {
        m_time.setStart( 0.0 );
        m_time.setEnd( 0.0 );
        clear();
    }

    /**
     * Return the number of FRAMEs.
     */
    inline uint size() const { return m_keyframe.size(); }

    /**
     * Returns true if there is at least one FRAME.
     */
    inline bool empty() const { return m_keyframe.empty(); }
    /// \}

    /// \name Comparison operators
    /// \{

    /**
     * Return true if *this and \p keyframe have the exact same AnimationTime and FRAMEs.
     */
    inline bool operator==( const KeyFrame& keyframe ) const {
        return ( ( m_time == keyframe.m_time ) && ( m_keyframe == keyframe.m_keyframe ) );
    }

    /**
     * Return true if *this and \p keyframe have either a different AnimationTime
     * or different FRAMEs.
     */
    inline bool operator!=( const KeyFrame& keyframe ) const { return !( *this == keyframe ); }
    /// \}

  protected:
    /**
     * Return the default FRAME value.
     */
    virtual FRAME defaultFrame() const = 0;

    /**
     * Returns the interpolation between \p F0 and \p F1, according to \p t.
     */
    virtual FRAME interpolate( const FRAME& F0, const FRAME& F1, const Scalar t ) const = 0;

  private:
    /**
     * For the given time \p t, find the FRAMEs \p F0 and \p F1 whose times surround \p t.
     * Returns in dt the linear interpolation parameter of \p t between those times,
     * i.e. \f$ t = t0 + dt * ( t1 - t0 ) \f$.
     */
    inline void findRange( const Time& t, FRAME& F0, FRAME& F1, Scalar& dt ) const {
        auto it = m_keyframe.find( t );
        // exact match
        if ( it != m_keyframe.end() )
        {
            F0 = it->second;
            F1 = it->second;
            dt = 0.0;
            return;
        }
        // before first
        if ( t < m_keyframe.begin()->first )
        {
            F0 = m_keyframe.begin()->second;
            F1 = F0;
            dt = 0.0;
            return;
        }
        // after last
        if ( t > m_keyframe.rbegin()->first )
        {
            F0 = m_keyframe.rbegin()->second;
            F1 = F0;
            dt = 0.0;
            return;
        }
        // in-between
        auto upper = m_keyframe.upper_bound( t );
        auto lower = upper;
        --lower;
        F0 = lower->second;
        Time t0 = lower->first;
        F1 = upper->second;
        Time t1 = upper->first;
        dt = ( t - t0 ) / ( t1 - t0 );
    }

  protected:
    /// The AnimationTime.
    AnimationTime m_time;

    /// The list of FRAMEs, mapped to their time instant.
    std::map<Time, FRAME, std::less<Time>,
             Ra::Core::AlignedAllocator<std::pair<const Time, FRAME>, EIGEN_MAX_ALIGN_BYTES>>
        m_keyframe;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_FRAME_HPP
