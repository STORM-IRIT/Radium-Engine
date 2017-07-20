#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Core/RaCore.hpp>
#include <Core/File/AssetData.hpp>
#include <Core/File/KeyFrame/AnimationTime.hpp>
#include <Core/File/KeyFrame/KeyTransform.hpp>

namespace Ra {
namespace Asset {

struct RA_CORE_API HandleAnimation {
    HandleAnimation( const std::string& name = "" );

    std::string   m_name;
    KeyTransform  m_anim;
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
    inline std::vector< HandleAnimation > getFrames() const;
    inline void setFrames( const std::vector< HandleAnimation >& frameList );

    /// DEBUG
    inline void displayInfo() const;

protected:
    /// VARIABLE
    AnimationTime                  m_time;
    Time                           m_dt;
    std::vector< HandleAnimation > m_keyFrame;
};

} // namespace Asset
} // namespace Ra

#include <Core/File/AnimationData.inl>

#endif // RADIUMENGINE_ANIMATION_DATA_HPP
