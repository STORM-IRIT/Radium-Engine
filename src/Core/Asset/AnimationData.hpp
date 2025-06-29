#pragma once

#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Asset/AnimationTime.hpp>
#include <Core/Asset/AssetData.hpp>
#include <Core/CoreMacros.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Log.hpp>
#include <algorithm>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * A HandleAnimation stores data for an animation Handle.
 */
struct RA_CORE_API HandleAnimation {
    explicit HandleAnimation( const std::string& name = "" );

    /// The Handle's name.
    std::string m_name;

    /// The list of KeyFramed transforms applied to the Handle.
    Core::Animation::KeyFramedValue<Transform> m_anim;

    /// The AnimationTime for the Handle.
    AnimationTime m_animationTime;
};

/**
 * The AnimationData class stores all the HandleAnimation related to an
 * animation of an object, one per animation Handle.
 * \note Objects can have several AnimationData, one for each animation.
 */
class RA_CORE_API AnimationData : public AssetData
{
  public:
    explicit AnimationData( const std::string& name = "" );

    ~AnimationData();

    /**
     * Sets the name of the animation.
     */
    inline void setName( const std::string& name ) { m_name = name; }

    /// \name Time
    /// \{

    /**
     * \returns the AnimationTime of the animation.
     */
    inline const AnimationTime& getTime() const { return m_time; }

    /**
     * Sets the AnimationTime of the animation.
     */
    inline void setTime( const AnimationTime& time ) { m_time = time; }
    /**
     * \returns the animation timestep.
     */
    inline AnimationTime::Time getTimeStep() const { return m_dt; }

    /**
     * Sets the animation timestep.
    ￼ */
    inline void setTimeStep( const AnimationTime::Time& delta ) { m_dt = delta; }

    /// \}

    /// \name Keyframes
    /// \{

    /**
     * \returns the number of HandleAnimations.
     */
    inline uint getFramesSize() const { return m_keyFrame.size(); }

    /**
     * \returns the list of HandleAnimations, i.e. the whole animation frames.
     */
    inline std::vector<HandleAnimation> getHandleData() const { return m_keyFrame; }

    /**
     * Sets the animation frames.
     */
    inline void setHandleData( const std::vector<HandleAnimation>& frameList );
    /// \}

    /**
     * Print stat info to the Debug output.
     */
    inline void displayInfo() const;

  protected:
    /// The AnimationTime for the object.
    AnimationTime m_time;

    /// The animation timestep.
    AnimationTime::Time m_dt { 0 };

    /// The animation frames.
    std::vector<HandleAnimation> m_keyFrame;
};

inline void AnimationData::setHandleData( const std::vector<HandleAnimation>& frameList ) {
    const uint size = frameList.size();
    m_keyFrame.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i ) {
        m_keyFrame[i] = frameList[i];
    }
}

inline void AnimationData::displayInfo() const {
    using namespace Core::Utils; // log
    LOG( logDEBUG ) << "======== ANIMATION INFO ========";
    LOG( logDEBUG ) << " Name              : " << m_name;
    LOG( logDEBUG ) << " Start Time        : " << m_time.getStart();
    LOG( logDEBUG ) << " End   Time        : " << m_time.getEnd();
    LOG( logDEBUG ) << " Time Step         : " << m_dt;
    LOG( logDEBUG ) << " Animated Object # : " << m_keyFrame.size();
}

} // namespace Asset
} // namespace Core
} // namespace Ra
