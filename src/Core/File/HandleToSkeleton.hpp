#ifndef RADIUMENGINE_HANDLE_TO_SKELETON_HPP_
#define RADIUMENGINE_HANDLE_TO_SKELETON_HPP_

#include <map>

#include <Core/RaCore.hpp>

namespace Ra{ namespace Core {namespace Animation { class Skeleton;}}}
namespace Ra{ namespace Asset { class HandleData;}}

namespace Ra {
namespace Asset {

/// Create a skeleton from a Handle Data extracted from a file
/// Outputs the skeleton and a map from the component index in the handle data to the bone index.
void RA_CORE_API createSkeleton( const Ra::Asset::HandleData& data, Core::Animation::Skeleton& skelOut,  std::map<uint,uint>& indexTableOut );

}
}



#endif // RADIUMENGINE_HANDLE_TO_SKELETON_HPP_
