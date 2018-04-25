#include <Core/Animation/BulgeCorrection.hpp>

#include <Core/Geometry/SegmentOperation.hpp>

namespace Ra {
namespace Core {
namespace Animation {

BulgeCorrectionData::BulgeCorrectionData() : m_prj(), m_dv() {}

BulgeCorrectionData::BulgeCorrectionData( const uint size ) : m_prj( size ), m_dv( size ) {}

void BulgeCorrectionData::resize( const uint size ) {
    m_prj.resize( size );
    m_dv.resize( size );
}

void bulgeCorrection( const Container::Vector3Array& restMesh, const BulgeCorrectionData& restData,
                      Container::Vector3Array& currMesh, const BulgeCorrectionData& currData ) {
    CORE_ASSERT( ( restMesh.size() == currMesh.size() ), " Meshes don't match " );
    const uint n = restMesh.size();
#pragma omp parallel for
    for ( int i = 0; i < int( n ); ++i )
    {
        if ( restData.m_dv[i] < currData.m_dv[i] )
        {
            const Math::Vector3 dir = currMesh[i] - currData.m_prj[i];
            const Scalar factor = std::sqrt( restData.m_dv[i] / currData.m_dv[i] );
            currMesh[i] = currData.m_prj[i] + ( factor * dir );
        }
    }
}

void findCorrectionData( const Container::Vector3Array& mesh, const MaxWeightID& wID,
                         const Utils::AdjacencyList& graph, const Pose& pose,
                         BulgeCorrectionData& data ) {
    const uint n = mesh.size();
    data.resize( n );
#pragma omp parallel for
    for ( int i = 0; i < int( n ); ++i )
    {
        Math::Vector3 start;
        Math::Vector3 end;
        const auto& child = graph.m_child[wID[i]];
        start = pose[wID[i]].translation();
        end.setZero();
        for ( const auto& c : child )
        {
            end += pose[c].translation();
        }
        data.m_prj[i] = Geometry::projectPointOnSegment( mesh[i], start, end );
        data.m_dv[i] = ( mesh[i] - data.m_prj[i] ).squaredNorm();
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
