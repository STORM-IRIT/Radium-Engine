#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Core/Asset/AssetData.hpp>
#include <Core/Asset/AnimationTime.hpp>
#include <Core/Asset/KeyTransform.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {

struct RA_CORE_API HandleAnimation {
    HandleAnimation( const std::string& name = "" );

    std::string m_name;
    KeyTransform m_anim;
};

class RA_CORE_API AnimationData : public AssetData {
  public:
    /// CONSTRUCTOR
    AnimationData( const std::string& name = "" );
    /// DESTRUCTOR
    ~AnimationData();

    /// NAME
    inline void setName( const std::string& name );

    /// TIME
    inline const AnimationTime& getTime() const;
    inline void setTime( const AnimationTime& time );
    inline Time getTimeStep() const;
    inline void setTimeStep( const Time& delta );

    /// KEY FRAME
    inline uint getFramesSize() const;
    inline std::vector<HandleAnimation> getFrames() const;
    inline void setFrames( const std::vector<HandleAnimation>& frameList );

    /// DEBUG
    inline void displayInfo() const;

  protected:
    /// VARIABLE
    AnimationTime m_time;
    Time m_dt;
    std::vector<HandleAnimation> m_keyFrame;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/AnimationData.inl>

#endif // RADIUMENGINE_ANIMATION_DATA_HPP
