#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Engine/Assets/AssimpAnimationDataLoader.hpp>
#include <Engine/Assets/KeyFrame/AnimationTime.hpp>
#include <Engine/Assets/KeyFrame/KeyTransform.hpp>

namespace Ra {
namespace Asset {

struct HandleAnimation {
    HandleAnimation( const std::string& name = "" );

    std::string   m_name;
    KeyTransform  m_anim;
};

class AnimationData {
public:
    /// FRIEND
    friend class AssimpAnimationDataLoader;

    /// CONSTRUCTOR
    AnimationData( const std::string& name = "" );
    /// DESTRUCTOR
    ~AnimationData();

    /// NAME
    inline std::string getName() const;

    /// TIME
    inline AnimationTime getTime() const;
    inline Time getTimeStep() const;

    /// KEY FRAME
    inline uint getFramesSize() const;
    inline std::vector< HandleAnimation > getFrames() const;

    /// DEBUG
    inline void displayInfo() const;

protected:
    /// NAME
    inline void setName( const std::string& name );

    /// TIME
    inline void setTime( const AnimationTime& time );

    inline void setTimeStep( const Time& delta );

    /// KEYFRAME
    inline void setFrames( const std::vector< HandleAnimation >& frameList );

protected:
    /// VARIABLE
    std::string                    m_name;
    AnimationTime                  m_time;
    Time                           m_dt;
    std::vector< HandleAnimation > m_keyFrame;
};

} // namespace Asset
} // namespace Ra

#include <Engine/Assets/AnimationData.inl>

#endif // RADIUMENGINE_ANIMATION_DATA_HPP
