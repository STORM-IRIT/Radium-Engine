#ifndef RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP
#define RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP

#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Animation {

using Dv = Vector1Array;
using BoneProjection = Vector3Array;

// Array containing the ID of the bone influencing the most a vertex.
using MaxWeightID = std::vector<uint>;

/// The BulgeCorrectionData stores all data used to perform the bulge
/// correction algorithm from https://onlinelibrary.wiley.com/doi/epdf/10.1002/cav.1604 .
struct BulgeCorrectionData {
    BulgeCorrectionData();
    BulgeCorrectionData( const uint size );
    BulgeCorrectionData( const BulgeCorrectionData& data ) = default;

    /// Resize data for match \p size.
    void resize( const uint size );

    /// List of point projections on bone segments.
    BoneProjection m_prj;

    /// List of distances from points to projections.
    Dv m_dv;
};

/// Apply the bulge correction to \p currMesh according to the initial mesh \p restMesh and
/// the initial and current BulgeCorrectionData.
void bulgeCorrection( const Vector3Array& restMesh, const BulgeCorrectionData& restData,
                      Vector3Array& currMesh, const BulgeCorrectionData& currData );

/// Computes the BulgeCorrectionData in \p data for the current mesh \p mesh.
void findCorrectionData( const Vector3Array& mesh, const MaxWeightID& wID,
                         const Graph::AdjacencyList& graph, const Pose& pose,
                         BulgeCorrectionData& data );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP
