#ifndef RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP
#define RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Utils/Graph/AdjacencyList.hpp>
#include <Core/Animation/Pose/Pose.hpp>


namespace Ra {
namespace Core {
namespace Animation {

typedef Vector1Array        Dv;
typedef Vector3Array        BoneProjection;
typedef std::vector< uint > MaxWeightID;    // Array containing the ID of the bone influencing the most a vertex

struct BulgeCorrectionData {
    BulgeCorrectionData();
    BulgeCorrectionData( const uint size );
    BulgeCorrectionData( const BulgeCorrectionData& data ) = default;

    void resize( const uint size );
    BoneProjection m_prj;
    Dv             m_dv;
};



void bulgeCorrection( const Vector3Array&        restMesh,
                      const BulgeCorrectionData& restData,
                      Vector3Array&              currMesh,
                      const BulgeCorrectionData& currData );



void findCorrectionData( const Vector3Array&         mesh,
                         const MaxWeightID&          wID,
                         const Graph::AdjacencyList& graph,
                         const Pose&                 pose,
                         BulgeCorrectionData&        data );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_BULGE_CORRECTION_DEFINITION_HPP

