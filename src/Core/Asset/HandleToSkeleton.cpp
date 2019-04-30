#include <Core/Asset/HandleToSkeleton.hpp>

#include <Core/Animation/Skeleton.hpp>
#include <Core/Asset/HandleData.hpp>
#include <set>

namespace Ra {
namespace Core {
namespace Asset {

namespace {
// Recursive function to add bones
void addBone( const int parent,  // index of parent bone
              const uint dataID, // index in map
              const Ra::Core::AlignedStdVector<Ra::Core::Asset::HandleComponentData>&
                  data,                                                       // handle bone data
              const Ra::Core::AlignedStdVector<Ra::Core::Vector2i>& edgeList, // list of edges
              std::vector<bool>& processed,        // which ids have been processed
              Core::Animation::Skeleton& skelOut ) // correspondance between bone name and bone idx
{
    if ( !processed[dataID] )
    {
        processed[dataID] = true;
        const auto& dd = data[dataID];
        uint index = skelOut.addBone( parent, dd.m_frame,
                                      Ra::Core::Animation::Handle::SpaceType::MODEL, dd.m_name );
        for ( const auto& edge : edgeList )
        {
            if ( edge[0] == dataID )
            {
                addBone( index, edge[1], data, edgeList, processed, skelOut );
            }
        }
    }
}
} // namespace

void createSkeleton( const Ra::Core::Asset::HandleData& data, Core::Animation::Skeleton& skelOut ) {
    const uint size = data.getComponentDataSize();
    auto component = data.getComponentData();

    std::set<uint> root;
    for ( uint i = 0; i < size; ++i )
    {
        root.insert( i );
    }

    auto edgeList = data.getEdgeData();
    for ( const auto& edge : edgeList )
    {
        root.erase( edge[1] );
    }

    std::vector<bool> processed( size, false );
    for ( const auto& r : root )
    {
        addBone( -1, r, component, edgeList, processed, skelOut );
    }
}
} // namespace Asset
} // namespace Core
} // namespace Ra
