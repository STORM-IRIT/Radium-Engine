#ifndef RADIUMENGINE_KEYFRAMEDVALUE_HPP
#define RADIUMENGINE_KEYFRAMEDVALUE_HPP

#include <map>
#include <set>

#include <Core/Containers/AlignedAllocator.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * KeyFramedValueBase defines the following concept:
 * any variable (e.g. transformation matrix, color, scalar value, flag) that
 * needs to be animated can be controlled by a `KeyFramedValue`, which inherits
 * KeyFramedValueBase (see dedicated documentation).
 *
 * KeyFramedValueBase defines the basic interface available to control keyframes:
 * insertKeyFrame, removeKeyFrame and moveKeyFrame.
 */
class RA_CORE_API KeyFramedValueBase
{
  public:
    KeyFramedValueBase() {}

    virtual ~KeyFramedValueBase() {}

    /**
     * \returns the number of keyframes.
     */
    virtual size_t size() const = 0;

    /**
     * \returns the ordered list of the points in time where a keyframe is defined.
     */
    virtual inline std::vector<Scalar> getTimes() const = 0;

    /**
     * Inserts a keyframe at time \p t.
     */
    virtual void insertKeyFrame( const Scalar& t ) = 0;

    /**
     * Removes the \p i-th keyframe.
     * \returns true if the keyframe has been removed, false otherwise.
     */
    virtual bool removeKeyFrame( size_t i ) = 0;

    /**
     * Moves the \p i-th keyframe to time \p t.
     * \returns true if the keyframe has been moved, false otherwise.
     */
    virtual bool moveKeyFrame( size_t i, const Scalar& t ) = 0;
};

/**
 * KeyFramedValue extends KeyFramedValueBase by implementing its interface,
 * adding management for the VALUE_TYPE KeyFramedValue::KeyFrame,
 * which are pairs (time,value).
 *
 * It also introduces the concept of KeyFramedValue::Interpolator:
 * a KeyFramedValue::Interpolator is a function that takes as input a collection
 * of KeyFramedValue::KeyFrame and returns the value for a given time t
 * (see for instance Ra::Core::Animation::linearInterpolate).
 */
template <typename VALUE_TYPE>
class KeyFramedValue : public KeyFramedValueBase
{
  public:
    /// The type for a keyframe.
    using KeyFrame = std::pair<Scalar, VALUE_TYPE>;

    /// The type for the keyframes container.
    using KeyFrames = std::vector<KeyFrame>;

    /// The type for interpolators.
    using Interpolator = std::function<VALUE_TYPE( const KeyFrames& /*frames*/, Scalar /*t*/ )>;

    /**
     * Creates a KeyFramedValue from a first keyframe.
     * \param frame the first keyframe value.
     * \param t the first keyframe point in time.
     * \param interpolator the function used to interpolate between keyframes.
     */
    KeyFramedValue( VALUE_TYPE frame,
                    Scalar t,
                    Interpolator interpolator = []( const KeyFrames& frames,
                                                    Scalar ) { return frames.begin()->second; } ) :
        KeyFramedValueBase(),
        m_interpolator( interpolator ) {
        insertKeyFrame( t, frame );
    }

    KeyFramedValue( const KeyFramedValue& keyframe ) = default;

    inline KeyFramedValue& operator=( const KeyFramedValue& keyframe ) = default;

    inline size_t size() const override final { return m_keyframes.size(); }

    inline std::vector<Scalar> getTimes() const override final {
        std::vector<Scalar> time( m_keyframes.size() );
#pragma omp parallel for
        for ( int i = 0; i < m_keyframes.size(); ++i )
        {
            time[i] = m_keyframes[i].first;
        }
        return time;
    }

    /// \name KeyFrame Management
    /// \{

    /**
     * \returns the \p i-th keyframe.
     */
    inline const KeyFrame& getKeyFrame( size_t i ) { return m_keyframes[i]; }

    /**
    ￼ * Inserts a new keyframe with value \p frame at time \p t.
    ￼ * \note If a keyframe already exists for \p t, it will be overwritten.
    ￼ */
    inline void insertKeyFrame( const Scalar& t, const VALUE_TYPE& frame ) {
        KeyFrame kf( t, frame );
        auto upper = std::upper_bound(
            m_keyframes.begin(), m_keyframes.end(), kf, []( const auto& a, const auto& b ) {
                return a.first < b.first;
            } );
        auto lower = upper;
        --lower;
        if ( upper == m_keyframes.end() ) { m_keyframes.insert( upper, kf ); }
        else if ( lower == m_keyframes.end() )
        { m_keyframes.insert( m_keyframes.begin(), kf ); }
        else if ( Math::areApproxEqual( lower->first, t ) )
        { lower->second = frame; }
        else
        { m_keyframes.insert( upper, kf ); }
    }

    /**
    ￼ * Inserts a keyframe at time \p t corresponding to the value interpolated at \p t.
    ￼ */
    inline void insertKeyFrame( const Scalar& t ) override final { insertKeyFrame( t, at( t ) ); }

    inline bool removeKeyFrame( size_t i ) override final {
        m_keyframes.erase( m_keyframes.begin() + i );
        return true;
    }

    inline bool moveKeyFrame( size_t i, const Scalar& t ) override final {
        KeyFrame kf = getKeyFrame( i );
        if ( !Ra::Core::Math::areApproxEqual( kf.first, t ) )
        {
            removeKeyFrame( i );
            insertKeyFrame( t, kf.second );
        }
        return true;
    }

    /**
    ￼ * \returns the value at time \p t, interpolated from the keyframes.
    ￼ * \note This method calls the interpolator.
    ￼ */
    inline VALUE_TYPE at( const Scalar& t ) const { return m_interpolator( m_keyframes, t ); }
    /// \}

    ///  \name Comparison operators
    /// \{

    /**
     * @returns true if *this and \p keyframe have the exact same keyframes,
     *          false otherwise.
     */
    inline bool operator==( const KeyFramedValue& keyframe ) const {
        return ( m_keyframes == keyframe.m_keyframes );
    }

    /**
     * @returns true if *this and \p keyframe do not have the exact same keyframes,
     *          false otherwise.
     */
    inline bool operator!=( const KeyFramedValue& keyframe ) const {
        return !( *this == keyframe );
    }
    /// \}

    /// \name Interpolator
    /// \{

    /**
     * Sets the function to interpolate keyframes.
     */
    inline void setInterpolator( Interpolator interpolator ) { m_interpolator = interpolator; }

    /**
     * \returns the function interpolate keyframes.
     */
    inline Interpolator getInterpolator() const { return m_interpolator; }
    /// \}

  protected:
    /// The list of keyframes.
    KeyFrames m_keyframes;

    /// The function to interpolate keyframes.
    Interpolator m_interpolator;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEYFRAMEDVALUE_HPP
