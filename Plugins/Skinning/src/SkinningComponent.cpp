#include <SkinningComponent.hpp>

#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Animation/Pose/PoseOperation.hpp>
#include <Core/Animation/Handle/Skeleton.hpp>

#include <DualQuaternionSkinning.hpp>

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

        m_refData.m_skeleton = skel;
        m_refData.m_referenceMesh = mesh;
        m_refData.m_refPose = refPose;
        m_refData.m_weights = weights;

        m_frameData.m_previousPose = refPose;

        m_frameData.m_doSkinning = false;

        m_skeletonGetter = ComponentMessenger::getInstance()->ComponentMessenger::getterCallback<Skeleton>( getEntity(), m_contentsName );
        m_verticesWriter = ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName+"v" );
        m_normalsWriter = ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName+"n" );


        m_DQ.resize( m_refData.m_weights.rows(), DualQuaternion( Quaternion( 0.0, 0.0, 0.0, 0.0 ),
                                                       Quaternion( 0.0 ,0.0, 0.0, 0.0 ) ) );

        m_isReady = true;
    }
}
void SkinningComponent::skin()
{
    CORE_ASSERT( m_isReady, "Skinning is not setup");

    const Skeleton* skel = static_cast<const Skeleton*>(m_skeletonGetter());
    m_frameData.m_currentPose = skel->getPose(SpaceType::MODEL);
    if ( !Ra::Core::Animation::areEqual( m_frameData.m_currentPose, m_frameData.m_previousPose))
    {
        m_frameData.m_doSkinning = true;
        Ra::Core::Vector3Array* vertices = static_cast<Ra::Core::Vector3Array* >(m_verticesWriter());
        CORE_ASSERT( vertices->size() == m_refData.m_referenceMesh.m_vertices.size(), "Inconsistent meshes");

        m_frameData.m_refToCurrentRelPose = Ra::Core::Animation::relativePose(m_frameData.m_currentPose, m_refData.m_refPose);
        m_frameData.m_prevToCurrentRelPose = Ra::Core::Animation::relativePose(m_frameData.m_currentPose, m_frameData.m_previousPose);

        computeDQ( m_frameData.m_refToCurrentRelPose, m_refData.m_weights, m_DQ );
        DualQuaternionSkinning( m_refData.m_referenceMesh.m_vertices, m_DQ, (*vertices) );
    }
}

void SkinningComponent::endSkinning()
{
    if (m_frameData.m_doSkinning)
    {
        const Ra::Core::Vector3Array* vertices = static_cast<Ra::Core::Vector3Array* >(m_verticesWriter());
        Ra::Core::Vector3Array* normals = static_cast<Ra::Core::Vector3Array* >(m_normalsWriter());
        Ra::Core::Geometry::uniformNormal( *vertices, m_refData.m_referenceMesh.m_triangles, *normals );

        m_frameData.m_previousPose = m_frameData.m_currentPose;
        m_frameData.m_doSkinning = false;
    }
}

void SkinningComponent::handleWeightsLoading(const Ra::Asset::HandleData *data)
{
    m_contentsName = data->getName();
    setupIO(m_contentsName);
}

void SkinningComponent::setupIO( const std::string& id )
{
    Ra::Engine::ComponentMessenger::GetterCallback dqOut = std::bind( &SkinningComponent::getDQ, this );
    Ra::Engine::ComponentMessenger::getInstance()->registerOutput<Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion>>( getEntity(), this, id, dqOut);

    Ra::Engine::ComponentMessenger::GetterCallback refData = std::bind( &SkinningComponent::getRefData, this );
    Ra::Engine::ComponentMessenger::getInstance()->registerOutput<Ra::Core::Skinning::RefData>( getEntity(), this, id, refData);

    Ra::Engine::ComponentMessenger::GetterCallback frameData = std::bind( &SkinningComponent::getFrameData, this );
    Ra::Engine::ComponentMessenger::getInstance()->registerOutput<Ra::Core::Skinning::FrameData>( getEntity(), this, id, frameData);
}


}
