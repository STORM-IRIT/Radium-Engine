#include <Engine/Assets/AnimationData.hpp>

namespace Ra {
namespace Asset {

HandleAnimation::HandleAnimation( const std::string& name ) :
    m_name( name ),
    m_anim() { }

/// CONSTRUCTOR
AnimationData::AnimationData( const std::string& name ) :
    m_name( name ),
    m_time(),
    m_dt( 0.0 ),
    m_keyFrame() { }

/// DESTRUCTOR
AnimationData::~AnimationData() { }

} // namespace Asset
} // namespace Ra

