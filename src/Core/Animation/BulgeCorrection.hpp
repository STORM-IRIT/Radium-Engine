#pragma once

#include <Core/Animation/Pose.hpp>
#include <Core/Containers/AdjacencyList.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

using Dv             = Vector1Array;
using BoneProjection = Vector3Array;
// Array containing the ID of the bone influencing the most a vertex
using MaxWeightID = std::vector<uint>;

struct BulgeCorrectionData {
    BulgeCorrectionData();
    explicit BulgeCorrectionData( const uint size );
    BulgeCorrectionData( const BulgeCorrectionData& data ) = default;

    void resize( const uint size );
    BoneProjection m_prj;
    Dv m_dv;
};

void bulgeCorrection( const Vector3Array& restMesh,
                      const BulgeCorrectionData& restData,
                      Vector3Array& currMesh,
                      const BulgeCorrectionData& currData );

void findCorrectionData( const Vector3Array& mesh,
                         const MaxWeightID& wID,
                         const AdjacencyList& graph,
                         const Pose& pose,
                         BulgeCorrectionData& data );

} // namespace Animation
} // namespace Core
} // namespace Ra
