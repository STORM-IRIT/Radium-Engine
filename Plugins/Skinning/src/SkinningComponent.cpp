#include <SkinningComponent.hpp>

#include <Core/Math/DualQuaternion.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Animation/Pose/PoseOperation.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

#include <Engine/Entity/ComponentMessenger.hpp>

namespace SkinningPlugin
{

void SkinningComponent::skin()
{
    // get the current animation data.
    Ra::Core::Animation::Skeleton skel;
    bool hasSkel = Ra::Engine::ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, skel);

    Ra::Core::Animation::WeightMatrix weights;
    bool hasWeights = Ra::Engine::ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, weights);

    Ra::Core::Animation::Pose refPose;
    bool hasRefPose = Ra::Engine::ComponentMessenger::getInstance()->get(getEntity(), m_contentsName, refPose);

    // get the mesh
    Ra::Core::TriangleMesh mesh;
    bool hasMesh = Ra::Engine::ComponentMessenger::getInstance()->get(getEntity(), m_contentsName,mesh);


    if (hasSkel && hasWeights && hasMesh && hasRefPose )
    {
        // First time we skin, set the reference mesh
        if (m_referenceMesh.m_vertices.size() == 0)
        {
            m_referenceMesh = mesh;
        }

        CORE_ASSERT(skel.size() == weights.cols(), "Weights are incompatible with bones");
        CORE_ASSERT(mesh.m_vertices.size() == weights.rows(), "Weights are incompatible with Mesh");
        CORE_ASSERT(mesh.m_vertices.size() == m_referenceMesh.m_vertices.size(), "Mesh incompatible with reference mesh");

        Ra::Core::Animation::Pose currentPose = skel.getPose(Ra::Core::Animation::Handle::SpaceType::MODEL);
        if ( !Ra::Core::Animation::areEqual(currentPose, refPose))
        {
            Ra::Core::Animation::Pose relativePose = Ra::Core::Animation::relativePose(currentPose, refPose);



            // Do DQS
            Ra::Core::AlignedStdVector< Ra::Core::DualQuaternion > DQ
                    ( weights.rows(), Ra::Core::DualQuaternion( Ra::Core::Quaternion( 0.0, 0.0, 0.0, 0.0 ),
                                                               Ra::Core::Quaternion( 0.0 , 0.0, 0.0, 0.0 ) ) );

            ON_DEBUG( std::vector<Scalar> weightCheck( weights.rows(), 0.f));
    #pragma omp parallel for
            for( int k = 0; k < weights.outerSize(); ++k )
            {
                Ra::Core::DualQuaternion q( relativePose[k]);
                for( Ra::Core::Animation::WeightMatrix::InnerIterator it( weights, k ); it; ++it)
                {
                    uint   i = it.row();
                    Scalar w = it.value();
                    const Ra::Core::DualQuaternion wq = q * w;
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

            Ra::Core::TriangleMesh newMesh = mesh;
            const uint nVerts = mesh.m_vertices.size();
    #pragma omp parallel for
            for (uint i = 0; i < nVerts; ++i )
            {
                newMesh.m_vertices[i] = DQ[i].transform(m_referenceMesh.m_vertices[i]);
                CORE_ASSERT(newMesh.m_vertices[i].allFinite(), "Infinite point in DQS");
            }

            Ra::Core::Geometry::uniformNormal( newMesh.m_vertices, newMesh.m_triangles, newMesh.m_normals );

            bool skinned = Ra::Engine::ComponentMessenger::getInstance()->set(getEntity(), m_contentsName, newMesh);
            CORE_ASSERT( skinned, "Mesh cannot be skinned. It could be because the mesh is set to nondeformable");
        }
    }
}

void SkinningComponent::handleWeightsLoading(const Ra::Asset::HandleData *data)
{
   m_contentsName = data->getName();
}


}
