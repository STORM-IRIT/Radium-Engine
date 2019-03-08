#ifndef RADIUMENGINE_HANDLE_TO_SKELETON_HPP_
#define RADIUMENGINE_HANDLE_TO_SKELETON_HPP_

#include <map>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Animation {
class Skeleton;
}
} // namespace Core
} // namespace Ra
namespace Ra {
namespace Core {
namespace Asset {

class HandleData;

/**
 * Create a Skeleton from a HandleData extracted from a file.
 * Outputs the Skeleton and a map from the component index in the HandleData
 * to the bone index.
 */
RA_CORE_API void createSkeleton( const Ra::Core::Asset::HandleData& data,
                                 Core::Animation::Skeleton& skelOut,
                                 std::map<uint, uint>& indexTableOut );

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_HANDLE_TO_SKELETON_HPP_
