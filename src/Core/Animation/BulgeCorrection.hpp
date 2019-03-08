#ifndef RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP
#define RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP

#include <Core/Animation/Pose.hpp>
#include <Core/Containers/AdjacencyList.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * The BulgeCorrectionData stores all data used to perform the bulge correction
 * algorithm from https://onlinelibrary.wiley.com/doi/epdf/10.1002/cav.1604 .
 */
struct RA_CORE_API BulgeCorrectionData {
    BulgeCorrectionData();
    BulgeCorrectionData( const uint size );
    BulgeCorrectionData( const BulgeCorrectionData& data ) = default;

    /// Resize data for match \p size.
    void resize( const uint size );

    /// List of point projections on bone segments.
    Vector3Array m_prj;

    /// List of distances from points to projections.
    Vector1Array m_dv;
};

/// \name BulgeFree Dual Quaternion Skinning
/// \{

/**
 * Apply the bulge correction to \p currMesh according to the initial mesh \p
 * restMesh and the initial and current BulgeCorrectionData.
 */
RA_CORE_API void bulgeCorrection( const Vector3Array& restMesh, const BulgeCorrectionData& restData,
                                  Vector3Array& currMesh, const BulgeCorrectionData& currData );

/**
 * Computes the BulgeCorrectionData in \p data for the current mesh \p mesh.
 */
RA_CORE_API void findCorrectionData( const Vector3Array& mesh, const std::vector<uint>& wID,
                                     const AdjacencyList& graph, const Pose& pose,
                                     BulgeCorrectionData& data );
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP
