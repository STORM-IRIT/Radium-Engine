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
 * The KeyFramedValueBase class is the base class for all KeyFramedValues.
 * It defines the basic interface all KeyFramedValues must implement.
 */
class RA_CORE_API KeyFramedValueBase
{
  public:
    KeyFramedValueBase() {}

    virtual ~KeyFramedValueBase() {}

    /**
     * \returns the ordered list of the points in time where a keyframe is defined.
     */
    virtual inline std::set<Scalar> getTimeSchedule() const = 0;

    /**
     * Inserts a keyframe at time \p t.
     */
    virtual void insertKeyFrame( const Scalar& t ) = 0;

    /**
     * Removes the keyframe at time \p t.
     */
    virtual bool removeKeyFrame( const Scalar& t ) = 0;

    /**
     * Moves the keyframe at time \p t0 to time \p t1.
     */
    virtual bool moveKeyFrame( const Scalar& t0, const Scalar& t1 ) = 0;
};

/**
 * The KeyFramedValue class is a generic container of keyframed values.
 * The VALUE_TYPE values are bound to a certain point in time.
 * \see Timeline And Keyframes documentation page for usage examples.
 */
template <typename VALUE_TYPE>
class KeyFramedValue : public KeyFramedValueBase
{
  public:
    /// The type for the keyframes container.
    using KeyFrames = std::map<
        Scalar,
        VALUE_TYPE,
        std::less<Scalar>,
        Ra::Core::AlignedAllocator<std::pair<const Scalar, VALUE_TYPE>, EIGEN_MAX_ALIGN_BYTES>>;

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
        m_keyframes[t] = frame;
    }

    KeyFramedValue( const KeyFramedValue& keyframe ) = default;

    inline KeyFramedValue& operator=( const KeyFramedValue& keyframe ) = default;

    inline std::set<Scalar> getTimeSchedule() const override final {
        std::set<Scalar> time;
        for ( const auto& kf : m_keyframes )
        {
            time.insert( kf.first );
        }
        return time;
    }

    /// \name KeyFrame Management
    /// \{

    /**
    ￼ * Inserts a new keyframe with value \p frame at time \p t.
    ￼ * \note If a keyframe already exists for \p t, it will be overwritten.
    ￼ */
    inline void insertKeyFrame( const Scalar& t, const VALUE_TYPE& frame ) {
        m_keyframes[t] = frame;
    }

    /**
    ￼ * Inserts a keyframe at time \p t corresponding to the value interpolated at \p t.
    ￼ */
    inline void insertKeyFrame( const Scalar& t ) override final { insertKeyFrame( t, at( t ) ); }

    inline bool removeKeyFrame( const Scalar& t ) override {
        if ( m_keyframes.size() == 1 ) { return false; }
        auto it = m_keyframes.find( t );
        if ( it == m_keyframes.end() ) { return false; }
        m_keyframes.erase( it );
        return true;
    }

    inline bool moveKeyFrame( const Scalar& t0, const Scalar& t1 ) override {
        if ( Ra::Core::Math::areApproxEqual( t0, t1 ) ) { return true; }
        auto it = m_keyframes.find( t0 );
        if ( it == m_keyframes.end() ) { return false; }
        insertKeyFrame( t1, it->second );
        m_keyframes.erase( it );
        return true;
    }

    /**
     * @returns the number of keyframes.
     */
    inline uint size() const { return m_keyframes.size(); }

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
