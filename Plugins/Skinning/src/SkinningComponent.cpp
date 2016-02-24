#include <SkinningComponent.hpp>

#include <Core/Math/DualQuaternion.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Animation/Pose/PoseOperation.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>


using Ra::Core::Quaternion;
using Ra::Core::DualQuaternion;

using Ra::Core::TriangleMesh;
using Ra::Core::Animation::Skeleton;
using Ra::Core::Animation::Pose;
using Ra::Core::Animation::WeightMatrix;
typedef Ra::Core::Animation::Handle::SpaceType SpaceType;

using Ra::Engine::ComponentMessenger;
namespace SkinningPlugin
{

void SkinningComponent::setupSkinning()
{
    // get the current animation data.
    Skeleton skel;
    bool hasSkel = ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, skel);

    WeightMatrix weights;
    bool hasWeights = ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, weights);

    Pose refPose;
    bool hasRefPose = ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, refPose);

    // get the mesh
    TriangleMesh mesh;
    bool hasMesh = ComponentMessenger::getInstance()->get(getEntity(), m_contentsName,mesh);


    if ( hasSkel && hasWeights && hasMesh && hasRefPose )
    {

        // Attempt to write to the mesh
        ON_DEBUG( bool skinnable = ComponentMessenger::getInstance()->set(getEntity(), m_contentsName, mesh ));
        CORE_ASSERT( skinnable, "Mesh cannot be skinned. It could be because the mesh is set to nondeformable" );
        CORE_ASSERT( skel.size() == weights.cols(), "Weights are incompatible with bones" );
        CORE_ASSERT( mesh.m_vertices.size() == weights.rows(), "Weights are incompatible with Mesh" );

        m_referenceMesh = mesh;
        m_refPose = refPose;
        m_previousPose = refPose;
        m_weights = weights;

        m_skeletonGetter = ComponentMessenger::getInstance()->ComponentMessenger::getterCallback<Skeleton>( getEntity(), m_contentsName );
        m_verticesWriter = ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName+"v" );
        m_normalsWriter = ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName+"n" );


        m_isReady = true;
    }
}
void SkinningComponent::skin()
{
    CORE_ASSERT( m_isReady, "Skinning is not setup");

    const Skeleton* skel = static_cast<const Skeleton*>(m_skeletonGetter());


    const Pose& currentPose = skel->getPose(SpaceType::MODEL);
    if ( !Ra::Core::Animation::areEqual(currentPose, m_previousPose))
    {
        m_vertices = static_cast<Ra::Core::Vector3Array* >(m_verticesWriter());
        m_normals = static_cast<Ra::Core::Vector3Array* >(m_normalsWriter());
        CORE_ASSERT( m_vertices->size() == m_referenceMesh.m_vertices.size(), "Inconsistent meshes");

        Pose relativePose = Ra::Core::Animation::relativePose(currentPose, m_refPose);

        // Do DQS
        Ra::Core::AlignedStdVector< DualQuaternion > DQ
                ( m_weights.rows(), DualQuaternion( Quaternion( 0.0, 0.0, 0.0, 0.0 ),
                                                    Quaternion( 0.0 ,0.0, 0.0, 0.0 ) ) );

        ON_DEBUG( std::vector<Scalar> weightCheck( m_weights.rows(), 0.f));
#pragma omp parallel for
        for( int k = 0; k < m_weights.outerSize(); ++k )
        {
            DualQuaternion q( relativePose[k]);
            for( WeightMatrix::InnerIterator it( m_weights, k ); it; ++it)
            {
                uint   i = it.row();
                Scalar w = it.value();
                const DualQuaternion wq = q * w;
#pragma omp critical
                {
                    DQ[i] += wq;
                    ON_DEBUG( weightCheck[i] += w);
                }
            }
        }

        // Normalize all dual quats.
#pragma omp parallel for
        for(uint i = 0; i < DQ.size() ; ++i)
        {
            DQ[i].normalize();

            CORE_ASSERT( Ra::Core::Math::areApproxEqual(weightCheck[i],1.f), "Incorrect skinning weights");
            CORE_ASSERT( DQ[i].getQ0().coeffs().allFinite() && DQ[i].getQe().coeffs().allFinite(),
                         "Invalid dual quaternion.");
        }

        const uint nVerts = m_referenceMesh.m_vertices.size();
#pragma omp parallel for
        for (uint i = 0; i < nVerts; ++i )
        {
            (*m_vertices)[i] = DQ[i].transform(m_referenceMesh.m_vertices[i]);
            CORE_ASSERT((*m_vertices)[i].allFinite(), "Infinite point in DQS");
        }

        Ra::Core::Geometry::uniformNormal( *m_vertices, m_referenceMesh.m_triangles, *m_normals );
        m_previousPose = currentPose;
    }
}

void SkinningComponent::handleWeightsLoading(const Ra::Asset::HandleData *data)
{
   m_contentsName = data->getName();
}


}
