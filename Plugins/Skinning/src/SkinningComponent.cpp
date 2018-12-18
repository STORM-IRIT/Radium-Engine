#include <SkinningComponent.hpp>

#include <Core/Animation/Pose/PoseOperation.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Mesh/MeshUtils.hpp>

#include <Core/Animation/Skinning/DualQuaternionSkinning.hpp>
#include <Core/Animation/Skinning/LinearBlendSkinning.hpp>
#include <Core/Animation/Skinning/RotationCenterSkinning.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

using Ra::Core::DualQuaternion;
using Ra::Core::Quaternion;

using Ra::Core::TriangleMesh;
using Ra::Core::Animation::Pose;
using Ra::Core::Animation::RefPose;
using Ra::Core::Animation::Skeleton;
using Ra::Core::Animation::WeightMatrix;

using SpaceType = Ra::Core::Animation::Handle::SpaceType;

using Ra::Core::Skinning::FrameData;
using Ra::Core::Skinning::RefData;

using Ra::Engine::ComponentMessenger;
namespace SkinningPlugin {

bool findDuplicates( const TriangleMesh& mesh, std::vector<Ra::Core::Index>& duplicatesMap ) {
    bool hasDuplicates = false;
    duplicatesMap.clear();
    const uint numVerts = mesh.vertices().size();
    duplicatesMap.resize( numVerts, Ra::Core::Index::Invalid() );

    Ra::Core::Vector3Array::const_iterator vertPos;
    Ra::Core::Vector3Array::const_iterator duplicatePos;
    std::vector<std::pair<Ra::Core::Vector3, Ra::Core::Index>> vertices;

    for ( uint i = 0; i < numVerts; ++i )
    {
        vertices.push_back( std::make_pair( mesh.vertices()[i], Ra::Core::Index( i ) ) );
    }

    std::sort( vertices.begin(), vertices.end(),
               []( std::pair<Ra::Core::Vector3, int> a, std::pair<Ra::Core::Vector3, int> b ) {
                   if ( a.first.x() == b.first.x() )
                   {
                       if ( a.first.y() == b.first.y() )
                           if ( a.first.z() == b.first.z() )
                               return a.second < b.second;
                           else
                               return a.first.z() < b.first.z();
                       else
                           return a.first.y() < b.first.y();
                   }
                   return a.first.x() < b.first.x();
               } );
    // Here vertices contains vertex pos and idx, with equal
    // vertices contiguous, sorted by idx, so checking if current
    // vertex equals the previous one state if its a duplicated
    // vertex position.
    duplicatesMap[vertices[0].second] = vertices[0].second;
    for ( uint i = 1; i < numVerts; ++i )
    {
        if ( vertices[i].first == vertices[i - 1].first )
        {
            duplicatesMap[vertices[i].second] = duplicatesMap[vertices[i - 1].second];
            hasDuplicates = true;
        } else
        { duplicatesMap[vertices[i].second] = vertices[i].second; }
    }

    return hasDuplicates;
}

void SkinningComponent::initialize() {
    auto compMsg = ComponentMessenger::getInstance();
    // get the current animation data.
    bool hasSkel = compMsg->canGet<Skeleton>( getEntity(), m_contentsName );
    bool hasWeights = compMsg->canGet<WeightMatrix>( getEntity(), m_contentsName );
    bool hasRefPose = compMsg->canGet<RefPose>( getEntity(), m_contentsName );
    bool hasMesh = compMsg->canGet<TriangleMesh>( getEntity(), m_contentsName );

    if ( hasSkel && hasWeights && hasMesh && hasRefPose )
    {
        m_skeletonGetter = compMsg->getterCallback<Skeleton>( getEntity(), m_contentsName );
        m_verticesWriter =
            compMsg->rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName + "v" );
        m_normalsWriter =
            compMsg->rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName + "n" );

        // copy mesh triangles and find duplicates for normal computation.
        const TriangleMesh& mesh = compMsg->get<TriangleMesh>( getEntity(), m_contentsName );
        m_refData.m_referenceMesh.copyBaseGeometry( mesh );
        findDuplicates( mesh, m_duplicatesMap );

        // get other data
        m_refData.m_skeleton = compMsg->get<Skeleton>( getEntity(), m_contentsName );
        m_refData.m_refPose = compMsg->get<RefPose>( getEntity(), m_contentsName );
        m_refData.m_weights = compMsg->get<WeightMatrix>( getEntity(), m_contentsName );

        m_frameData.m_previousPose = m_refData.m_refPose;
        m_frameData.m_frameCounter = 0;
        m_frameData.m_doSkinning = false;
        m_frameData.m_doReset = false;
        m_frameData.m_previousPose = m_refData.m_refPose;
        m_frameData.m_currentPose  = m_refData.m_refPose;

        m_frameData.m_previousPos = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentPos = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentNormal = m_refData.m_referenceMesh.normals();

        m_frameData.m_refToCurrentRelPose  = Ra::Core::Animation::relativePose(
            m_frameData.m_currentPose, m_refData.m_refPose );
        m_frameData.m_prevToCurrentRelPose = Ra::Core::Animation::relativePose(
            m_frameData.m_currentPose, m_frameData.m_previousPose );

        // Do some debug checks:  Attempt to write to the mesh and check the weights match skeleton
        // and mesh.
        ON_ASSERT( bool skinnable =
                       compMsg->canSet<Ra::Core::TriangleMesh>( getEntity(), m_contentsName ) );
        CORE_ASSERT(
            skinnable,
            "Mesh cannot be skinned. It could be because the mesh is set to nondeformable" );
        CORE_ASSERT( m_refData.m_skeleton.size() == m_refData.m_weights.cols(),
                     "Weights are incompatible with bones" );
        CORE_ASSERT( m_refData.m_referenceMesh.vertices().size() == m_refData.m_weights.rows(),
                     "Weights are incompatible with Mesh" );

        m_isReady = true;
        setupSkinningType( m_skinningType );
    }
}

void SkinningComponent::skin() {
    CORE_ASSERT( m_isReady, "Skinning is not setup" );

    const Skeleton* skel = m_skeletonGetter();

    bool reset = ComponentMessenger::getInstance()->get<bool>( getEntity(), m_contentsName );

    // Reset the skin if it wasn't done before
    if ( reset && !m_frameData.m_doReset )
    {
        m_frameData.m_doReset = true;
        m_frameData.m_frameCounter = 0;
    } else
    {
        m_frameData.m_currentPose = skel->getPose( SpaceType::MODEL );
        if ( !Ra::Core::Animation::areEqual( m_frameData.m_currentPose,
                                             m_frameData.m_previousPose ) )
        {
            m_frameData.m_doSkinning = true;
            m_frameData.m_frameCounter++;
            m_frameData.m_refToCurrentRelPose =
                Ra::Core::Animation::relativePose( m_frameData.m_currentPose, m_refData.m_refPose );
            m_frameData.m_prevToCurrentRelPose = Ra::Core::Animation::relativePose(
                m_frameData.m_currentPose, m_frameData.m_previousPose );

            switch ( m_skinningType )
            {
            case LBS:
            {
                Ra::Core::Animation::linearBlendSkinning(
                    m_refData.m_referenceMesh.vertices(), m_frameData.m_refToCurrentRelPose,
                    m_refData.m_weights, m_frameData.m_currentPos );
                break;
            }
            case DQS:
            {
                Ra::Core::AlignedStdVector<DualQuaternion> DQ;
                Ra::Core::Animation::computeDQ( m_frameData.m_refToCurrentRelPose,
                                                m_refData.m_weights, DQ );
                Ra::Core::Animation::dualQuaternionSkinning( m_refData.m_referenceMesh.vertices(),
                                                             DQ, m_frameData.m_currentPos );
                break;
            }
            case COR:
            {
                Ra::Core::Animation::corSkinning(
                    m_refData.m_referenceMesh.vertices(), m_frameData.m_refToCurrentRelPose,
                    m_refData.m_weights, m_refData.m_CoR, m_frameData.m_currentPos );
                break;
            }
            }
            Ra::Core::Animation::computeDQ( m_frameData.m_refToCurrentRelPose, m_refData.m_weights,
                                            m_DQ );
        }
    }
}

void uniformNormal( const Ra::Core::Vector3Array& p, const Ra::Core::VectorArray< Ra::Core::Vector3ui>& T,
                    const std::vector<Ra::Core::Index>& duplicateTable, Ra::Core::Vector3Array& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Ra::Core::Vector3::Zero() );

    for ( const auto& t : T )
    {
        const Ra::Core::Index i = duplicateTable.at( t( 0 ) );
        const Ra::Core::Index j = duplicateTable.at( t( 1 ) );
        const Ra::Core::Index k = duplicateTable.at( t( 2 ) );
        const Ra::Core::Vector3 triN = Ra::Core::Geometry::triangleNormal( p[i], p[j], p[k] );
        if ( !triN.allFinite() )
        {
            continue;
        }
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }

#pragma omp parallel for
    for ( uint i = 0; i < N; ++i )
    {
        if ( !normal[i].isApprox( Ra::Core::Vector3::Zero() ) )
        {
            normal[i].normalize();
        }
    }

#pragma omp parallel for
    for ( uint i = 0; i < N; ++i )
    {
        normal[i] = normal[duplicateTable[i]];
    }
}

void SkinningComponent::endSkinning() {
    if ( m_frameData.m_doSkinning )
    {
        Ra::Core::Vector3Array& vertices = *( m_verticesWriter() );
        Ra::Core::Vector3Array& normals = *( m_normalsWriter() );

        vertices = m_frameData.m_currentPos;

        // FIXME: normals should be computed by the Skinning method!
        uniformNormal( vertices, m_refData.m_referenceMesh.m_triangles,
                       m_duplicatesMap, normals );

        std::swap( m_frameData.m_previousPose, m_frameData.m_currentPose );
        std::swap( m_frameData.m_previousPos, m_frameData.m_currentPos );

        m_frameData.m_doSkinning = false;
    } else if ( m_frameData.m_doReset )
    {
        // Reset mesh to its initial state.
        Ra::Core::Vector3Array& vertices = *( m_verticesWriter() );
        Ra::Core::Vector3Array& normals = *( m_normalsWriter() );

        vertices = m_refData.m_referenceMesh.vertices();
        normals = m_refData.m_referenceMesh.normals();

        m_frameData.m_doReset = false;
        m_frameData.m_currentPose = m_refData.m_refPose;
        m_frameData.m_previousPose = m_refData.m_refPose;
        m_frameData.m_currentPos = m_refData.m_referenceMesh.vertices();
        m_frameData.m_previousPos = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentNormal = m_refData.m_referenceMesh.normals();
    }
}

void SkinningComponent::handleWeightsLoading( const Ra::Asset::HandleData* data ) {
    m_contentsName = data->getName();
    setupIO( m_contentsName );
}

void SkinningComponent::setContentsName( const std::string name ) {
    m_contentsName = name;
}

void SkinningComponent::setupIO( const std::string& id ) {
    using DualQuatVector = Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion>;

    ComponentMessenger::CallbackTypes<DualQuatVector>::Getter dqOut =
        std::bind( &SkinningComponent::getDQ, this );
    ComponentMessenger::getInstance()->registerOutput<DualQuatVector>( getEntity(), this, id,
                                                                       dqOut );

    ComponentMessenger::CallbackTypes<RefData>::Getter refData =
        std::bind( &SkinningComponent::getRefData, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Skinning::RefData>(
        getEntity(), this, id, refData );

    ComponentMessenger::CallbackTypes<FrameData>::Getter frameData =
        std::bind( &SkinningComponent::getFrameData, this );
    ComponentMessenger::getInstance()->registerOutput<FrameData>( getEntity(), this, id,
                                                                  frameData );
}

void SkinningComponent::setSkinningType( SkinningType type ) {
    m_skinningType = type;
    if ( m_isReady )
    {
        setupSkinningType( type );
    }
}

void SkinningComponent::setupSkinningType( SkinningType type ) {
    CORE_ASSERT( m_isReady, "component is not ready" );
    switch ( type )
    {
    case LBS:
        break;
    case DQS:
    {
        if ( m_DQ.empty() )
        {
            m_DQ.resize( m_refData.m_weights.rows(),
                         DualQuaternion( Quaternion( 0.0, 0.0, 0.0, 0.0 ),
                                         Quaternion( 0.0, 0.0, 0.0, 0.0 ) ) );
        }
        break;
    }
    case COR:
    {
        if ( m_refData.m_CoR.empty() )
        {
            Ra::Core::Animation::computeCoR( m_refData );
        }
    }
    } // end of switch.
}

} // namespace SkinningPlugin
