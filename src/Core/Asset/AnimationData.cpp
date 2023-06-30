#include <Core/Asset/AnimationData.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

void AnimationData::displayInfo() const {
    using namespace Core::Utils; // log
    LOG( logDEBUG ) << "======== ANIMATION INFO ========";
    LOG( logDEBUG ) << " Name              : " << getName();
    LOG( logDEBUG ) << " Start Time        : " << m_time.getStart();
    LOG( logDEBUG ) << " End   Time        : " << m_time.getEnd();
    LOG( logDEBUG ) << " Time Step         : " << m_dt;
    LOG( logDEBUG ) << " Animated Object # : " << m_keyFrame.size();
}
} // namespace Asset
} // namespace Core
} // namespace Ra
