#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Core/Animation/AnimationTime.hpp>
#include <Core/Animation/KeyTransform.hpp>
#include <Core/Asset/AssetData.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {

struct RA_CORE_API HandleAnimation {
    explicit HandleAnimation( const std::string& name = "" );

    std::string m_name;
    Core::Animation::KeyTransform m_anim;
};

class RA_CORE_API AnimationData : public AssetData
{
  public:
    /// CONSTRUCTOR
    explicit AnimationData( const std::string& name = "" );
    /// DESTRUCTOR
    ~AnimationData();

    /// NAME
    inline void setName( const std::string& name );

    /// TIME
    inline const Core::Animation::AnimationTime& getTime() const;
    inline void setTime( const Core::Animation::AnimationTime& time );
    inline Core::Animation::Time getTimeStep() const;
    inline void setTimeStep( const Core::Animation::Time& delta );

    /// KEY FRAME
    inline uint getFramesSize() const;
    inline std::vector<HandleAnimation> getFrames() const;
    inline void setFrames( const std::vector<HandleAnimation>& frameList );

    /// DEBUG
    inline void displayInfo() const;

  protected:
    /// VARIABLE
    Core::Animation::AnimationTime m_time;
    Core::Animation::Time m_dt;
    std::vector<HandleAnimation> m_keyFrame;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/AnimationData.inl>

#endif // RADIUMENGINE_ANIMATION_DATA_HPP
