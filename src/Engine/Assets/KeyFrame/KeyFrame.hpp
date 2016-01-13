#ifndef RADIUMENGINE_KEY_FRAME_HPP
#define RADIUMENGINE_KEY_FRAME_HPP

#include <map>
#include <set>
#include <Engine/Assets/KeyFrame/AnimationTime.hpp>

namespace Ra {
namespace Asset {

template < class FRAME >
class KeyFrame {
public:
    /// CONSTRUCTOR
    KeyFrame( const AnimationTime& time = AnimationTime() ) : m_time( time ) { }
    KeyFrame( const KeyFrame& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyFrame() { }

    /// TIME
    inline AnimationTime getAnimationTime() const {
        return m_time;
    }

    inline void setAnimationTime( const AnimationTime& time ) {
        m_time = time;
    }

    /// TRANSFORMATION
    inline FRAME getKeyFrame( const uint i ) const {
        CORE_ASSERT( ( i < size() ), "Index i out of bound" );
        return ( m_keyframe.begin() + i )->second;
    }

    inline FRAME& getKeyFrame( const uint i ) {
        CORE_ASSERT( ( i < size() ), "Index i out of bound" );
        return ( m_keyframe.begin() + i )->second;
    }

    inline FRAME at( const Time& t ) const {
        if( !m_time.contain( t ) ) {
            return defaultFrame();
        }
        FRAME F0;
        FRAME F1;
        Scalar dt;
        findRange( t, F0, F1, dt );
        return interpolate( F0, F1, dt );
    }

    inline void setKeyFrame( const uint i, const FRAME& frame ) {
        CORE_ASSERT( ( i < size() ), "Index i out of bound" );
        Time t = ( m_keyframe.begin() + i )->first;
        setKeyFrame( t, frame );
    }

    inline void setKeyFrame( const Time& t, const FRAME& frame ) {
        if( !m_time.contain( t ) ) {
            insertKeyFrame( t, frame );
        } else {
            m_keyframe[t] = frame;
        }
    }

    inline void insertKeyFrame( const Time& t,
                                const FRAME& frame ) {
        if( t < m_time.getStart() ) {
            m_time.setStart( t );
        }
        if( t > m_time.getEnd() ) {
            m_time.setEnd( t );
        }
        m_keyframe[t] = frame;
    }

    /// SIZE
    inline uint size() const {
        return m_keyframe.size();
    }

    inline void clear() {
        m_keyframe.clear();
    }

    /// QUERY
    inline bool empty() const {
        return m_keyframe.empty();
    }

    inline std::set< Time > timeSchedule() const {
        std::set< Time > time;
        for( const auto& it : m_keyframe ) {
            time.insert( it->first );
        }
        return time;
    }

    /// RESET
    inline void reset() {
        m_time.setStart( 0.0 );
        m_time.setEnd( 0.0 );
        clear();
    }

    /// OPERATOR
    inline KeyFrame& operator =( const KeyFrame& keyframe ) { m_time = keyframe.m_time; m_keyframe = keyframe.m_keyframe; return *this; }
//    inline KeyFrame  operator[]( const uint i ) const { return getKeyFrame( i ); }
//    inline KeyFrame& operator[]( const uint i ) { return getKeyFrame( i ); }
//    inline KeyFrame  operator[]( const Time& t ) const { return at( t ); }
    inline bool      operator==( const KeyFrame& keyframe ) const { return ( ( m_time == keyframe.m_time ) && ( m_keyframe == keyframe.m_keyframe ) ); }

protected:
    /// TRANSFORMATION
    virtual FRAME defaultFrame() const = 0;

    inline void findRange( const Time& t,
                           FRAME& F0,
                           FRAME& F1,
                           Scalar& dt ) const {
        auto it = m_keyframe.find( t );
        if( it != m_keyframe.end() ) {
            F0 = it->second;
            F1 = it->second;
            dt = 0.0;
            return;
        }
        auto lower = m_keyframe.lower_bound( t );
        auto upper = m_keyframe.upper_bound( t );
        F0 = lower->second;
        F1 = upper->second;
        dt = ( t - lower->first ) / ( upper->first - lower->first );
    }

    virtual FRAME interpolate( const FRAME& F0, const FRAME& F1, const Scalar t ) const = 0;

protected:
    /// VARIABLE
    AnimationTime           m_time;
    std::map< Time, FRAME > m_keyframe;
};



} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_KEY_FRAME_HPP
