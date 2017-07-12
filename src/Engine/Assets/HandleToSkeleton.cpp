#include <Engine/Assets/HandleToSkeleton.hpp>

#include <Core/Animation/Handle/Skeleton.hpp>
#include <Engine/Assets/HandleData.hpp>

namespace Ra {
namespace Asset{

namespace {
// Recursive function to add bones
void addBone( const int parent, // index of parent bone
              const uint dataID, // index in map
              const Ra::Core::AlignedStdVector< Ra::Asset::HandleComponentData >& data, // handle bone data
              const Ra::Core::AlignedStdVector< Ra::Core::Vector2i >& edgeList, //list of edges
              std::vector< bool >& processed, // which ids have been processed
              Core::Animation::Skeleton& skelOut, // skeleton being built
              std::map< uint, uint >& indexTable ) // correspondance between data idx and bone idx
{
    if( !processed[dataID] )
    {
        processed[dataID] = true;
        uint index = skelOut.addBone( parent, data.at( dataID ).m_frame,
                                      Ra::Core::Animation::Handle::SpaceType::MODEL, data.at( dataID ).m_name );
        indexTable[dataID] = index;
        for( const auto& edge : edgeList )
        {
            if( edge[0] == dataID )
            {
                addBone( index, edge[1], data, edgeList, processed, skelOut, indexTable );
            }
        }
    }
}
}

void createSkeleton( const Ra::Asset::HandleData& data, Core::Animation::Skeleton& skelOut,  std::map<uint,uint>& indexTableOut )
{
    const uint size = data.getComponentDataSize();
    auto component = data.getComponentData();

    std::set< uint > root;
    for( uint i = 0; i < size; ++i )
    {
        root.insert( i );
    }

    auto edgeList = data.getEdgeData();
    for( const auto& edge : edgeList )
    {
        root.erase( edge[1] );
    }

    std::vector< bool > processed( size, false );
    for( const auto& r : root )
    {
        addBone( -1, r, component, edgeList, processed, skelOut, indexTableOut );
    }
}
}
}
