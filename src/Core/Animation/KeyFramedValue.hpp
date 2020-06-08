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
 */
class RA_CORE_API KeyFramedValueBase
{
  public:
    KeyFramedValueBase() {}

    virtual ~KeyFramedValueBase() {}

    /**
     * @returns the number of keyframes.
     */
    virtual inline size_t size() const = 0;

    /**
     * Removes the \p i-th keyframe, if not the only one.
     * \returns true if the keyframed has been removed, false otherwise.
     */
    virtual inline bool removeKeyFrame( size_t i ) = 0;

    /**
     * Moves the \p i-th keyframe to time \p t.
     */
    virtual inline void moveKeyFrame( size_t i, const Scalar& t ) = 0;

    /**
     * \returns the ordered list of the points in time where a keyframe is defined.
     */
    virtual inline std::vector<Scalar> getTimes() const = 0;
};

/**
 * KeyFramedValue extends KeyFramedValueBase by implementing its interface,
 * adding management for the VALUE_TYPE KeyFramedValue::KeyFrame,
 * which are pairs (time,value) sorted by time.
 *
 * It also introduces the concept of Interpolator:
 * an Interpolator is a function that takes as input a collection
 * of KeyFramedValue::KeyFrame and returns the value for a given time t
 * (see for instance Ra::Core::Animation::linearInterpolate).
 *
 * \note There is always at least one keyframe defined.
 */
template <typename VALUE_TYPE>
class KeyFramedValue : public KeyFramedValueBase
{
  public:
    /// The type for a keyframe.
    using KeyFrame = std::pair<Scalar, VALUE_TYPE>;

    /// The type for the keyframes container.
    using KeyFrames = std::vector<KeyFrame>;

    /**
     * Creates a KeyFramedValue from a first keyframe.
     * \param frame the first keyframe value.
     * \param t the first keyframe point in time.
     * \param interpolator the function used to interpolate between keyframes.
     */
    KeyFramedValue( VALUE_TYPE frame, Scalar t ) { insertKeyFrame( t, frame ); }

    KeyFramedValue( const KeyFramedValue& keyframe ) = default;

    inline KeyFramedValue& operator=( const KeyFramedValue& keyframe ) = default;

    virtual inline std::vector<Scalar> getTimes() const {
        std::vector<Scalar> times( m_keyframes.size() );
#pragma omp parallel for
        for ( int i = 0; i < int( m_keyframes.size() ); ++i )
        {
            times[i] = m_keyframes[i].first;
        }
        return times;
    }

    /// \name KeyFrame Management
    /// \{

    /**
     * @returns the number of keyframes.
     */
    inline size_t size() const override { return m_keyframes.size(); }

    /**
     * @returns the collection of keyframes.
     */
    const KeyFrames& getKeyFrames() const { return m_keyframes; }

    /**
     * \returns the \p i-th keyframe.
     */
    inline const KeyFrame& operator[]( size_t i ) const { return m_keyframes[i]; }

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
        if ( upper == m_keyframes.begin() ) { m_keyframes.insert( upper, kf ); }
        else if ( Math::areApproxEqual( lower->first, t ) )
        { lower->second = frame; }
        else
        { m_keyframes.insert( upper, kf ); }
    }

    /**
    ￼ * Inserts a keyframe at time \p t corresponding to the value interpolated at \p t
     * using the given interpolator.
    ￼ */
    template <typename INTERPOLATOR>
    inline void insertInterpolatedKeyFrame( const Scalar& t, const INTERPOLATOR& interpolator ) {
        insertKeyFrame( t, at( t, interpolator ) );
    }

    /**
     * Removes the \p i-th keyframe, if not the only one.
     * \returns true if the keyframed has been removed, false otherwise.
     */
    inline bool removeKeyFrame( size_t i ) override {
        if ( size() == 1 ) return false;
        m_keyframes.erase( m_keyframes.begin() + i );
        return true;
    }

    /**
     * Moves the \p i-th keyframe to time \p t.
     */
    inline void moveKeyFrame( size_t i, const Scalar& t ) override {
        KeyFrame kf = m_keyframes[i];
        if ( !Ra::Core::Math::areApproxEqual( kf.first, t ) )
        {
            removeKeyFrame( i );
            insertKeyFrame( t, kf.second );
        }
    }

    /**
    ￼ * \returns the value at time \p t, interpolated from the keyframes using the
     *          given interpolator.
    ￼ */
    template <typename INTERPOLATOR>
    inline VALUE_TYPE at( const Scalar& t, const INTERPOLATOR& interpolator ) const {
        return interpolator( m_keyframes, t );
    }
    /// \}

    /// \name Comparison operators
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

  protected:
    /// The list of keyframes.
    KeyFrames m_keyframes;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEYFRAMEDVALUE_HPP
