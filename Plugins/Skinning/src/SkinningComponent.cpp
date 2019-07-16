#include <SkinningComponent.hpp>

#include <Core/Animation/PoseOperation.hpp>
#include <Core/Geometry/Normal.hpp>

#include <Core/Animation/DualQuaternionSkinning.hpp>
#include <Core/Animation/HandleWeightOperation.hpp>
#include <Core/Animation/LinearBlendSkinning.hpp>
#include <Core/Animation/RotationCenterSkinning.hpp>
#include <Core/Animation/StretchableTwistableBoneSkinning.hpp>
#include <Core/Geometry/DistanceQueries.hpp>
#include <Core/Geometry/TriangleOperation.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Resources/Resources.hpp>

#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

using Ra::Core::DualQuaternion;
using Ra::Core::Quaternion;

using Ra::Core::Animation::Pose;
using Ra::Core::Animation::RefPose;
using Ra::Core::Animation::Skeleton;
using Ra::Core::Animation::WeightMatrix;
using Ra::Core::Geometry::TriangleMesh;

using SpaceType = Ra::Core::Animation::Handle::SpaceType;

using Ra::Core::Skinning::FrameData;
using Ra::Core::Skinning::RefData;

using Ra::Engine::ComponentMessenger;

using namespace Ra::Core::Utils;

namespace SkinningPlugin {

bool findDuplicates( const TriangleMesh& mesh,
                     std::vector<Ra::Core::Utils::Index>& duplicatesMap ) {
    bool hasDuplicates = false;
    duplicatesMap.clear();
    const uint numVerts = mesh.vertices().size();
    duplicatesMap.resize( numVerts, Ra::Core::Utils::Index::Invalid() );

    Ra::Core::Vector3Array::const_iterator vertPos;
    Ra::Core::Vector3Array::const_iterator duplicatePos;
    std::vector<std::pair<Ra::Core::Vector3, Ra::Core::Utils::Index>> vertices;

    for ( uint i = 0; i < numVerts; ++i )
    {
        vertices.push_back( std::make_pair( mesh.vertices()[i], Ra::Core::Utils::Index( i ) ) );
    }

    std::sort( vertices.begin(),
               vertices.end(),
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
            hasDuplicates                     = true;
        }
        else
        { duplicatesMap[vertices[i].second] = vertices[i].second; }
    }

    return hasDuplicates;
}

void SkinningComponent::initialize() {
    auto compMsg = ComponentMessenger::getInstance();
    // get the current animation data.
    bool hasSkel    = compMsg->canGet<Skeleton>( getEntity(), m_contentsName );
    bool hasRefPose = compMsg->canGet<RefPose>( getEntity(), m_contentsName );
    bool hasMesh    = compMsg->canGet<TriangleMesh>( getEntity(), m_meshName );

    if ( hasSkel && hasMesh && hasRefPose )
    {
        m_renderObjectReader =
            compMsg->getterCallback<Ra::Core::Utils::Index>( getEntity(), m_meshName );
        m_skeletonGetter = compMsg->getterCallback<Skeleton>( getEntity(), m_contentsName );
        m_verticesWriter =
            compMsg->rwCallback<Ra::Core::Vector3Array>( getEntity(), m_meshName + "v" );
        m_normalsWriter =
            compMsg->rwCallback<Ra::Core::Vector3Array>( getEntity(), m_meshName + "n" );
        m_meshWritter = compMsg->rwCallback<TriangleMesh>( getEntity(), m_meshName );

        // copy mesh triangles and find duplicates for normal computation.
        TriangleMesh* mesh = const_cast<TriangleMesh*>( m_meshWritter() );
        m_refData.m_referenceMesh.copyBaseGeometry( *mesh );
        findDuplicates( *mesh, m_duplicatesMap );

        // get other data
        m_refData.m_skeleton       = compMsg->get<Skeleton>( getEntity(), m_contentsName );
        m_refData.m_refPose        = compMsg->get<RefPose>( getEntity(), m_contentsName );
        m_frameData.m_previousPose = m_refData.m_refPose;
        m_frameData.m_frameCounter = 0;
        m_frameData.m_doSkinning   = false;
        m_frameData.m_doReset      = false;
        m_frameData.m_previousPose = m_refData.m_refPose;
        m_frameData.m_currentPose  = m_refData.m_refPose;

        m_frameData.m_previousPos   = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentPos    = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentNormal = m_refData.m_referenceMesh.normals();

        m_frameData.m_refToCurrentRelPose =
            Ra::Core::Animation::relativePose( m_frameData.m_currentPose, m_refData.m_refPose );
        m_frameData.m_prevToCurrentRelPose = Ra::Core::Animation::relativePose(
            m_frameData.m_currentPose, m_frameData.m_previousPose );

        createWeightMatrix();

        // Do some debug checks:  Attempt to write to the mesh and check the weights match skeleton
        // and mesh.
        ON_ASSERT( bool skinnable = compMsg->canRw<Ra::Core::Geometry::TriangleMesh>(
                       getEntity(), m_meshName ) );
        CORE_ASSERT(
            skinnable,
            "Mesh cannot be skinned. It could be because the mesh is set to nondeformable" );
        CORE_ASSERT( m_refData.m_skeleton.size() == m_refData.m_weights.cols(),
                     "Weights are incompatible with bones" );
        CORE_ASSERT( m_refData.m_referenceMesh.vertices().size() == m_refData.m_weights.rows(),
                     "Weights are incompatible with Mesh" );

        m_isReady = true;
        setupSkinningType( m_skinningType );
        setupSkinningType( STBS_LBS ); // ensure weights are present for display

        // prepare RO for skinning weights display
        auto ro         = getRoMgr()->getRenderObject( *m_renderObjectReader() );
        m_baseTechnique = ro->getRenderTechnique();

        auto attrUV = Ra::Engine::Mesh::getAttribName( Ra::Engine::Mesh::VERTEX_TEXCOORD );
        if ( mesh->hasAttrib( attrUV ) )
        {
            auto handle = mesh->getAttribHandle<Ra::Core::Vector3>( attrUV );
            m_baseUV    = mesh->getAttrib( handle ).data();
        }

        m_weightTechnique.reset( new Ra::Engine::RenderTechnique );
        auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
        builder.second( *m_weightTechnique.get(), true );
        auto matT = std::static_pointer_cast<Ra::Engine::BlinnPhongMaterial>(
            ro->getRenderTechnique()->getMaterial() );
        std::shared_ptr<Ra::Engine::BlinnPhongMaterial> mat(
            new Ra::Engine::BlinnPhongMaterial( "IS2016_Mat" ) );
        mat->m_kd = Ra::Core::Utils::Color::Skin();
        mat->m_ks = Ra::Core::Utils::Color::White();
        m_weightTechnique->setMaterial( mat );
        // assign texture
        Ra::Engine::TextureParameters texParam;
        texParam.name = ":/Assets/Textures/Influence0.png";
        auto tex      = Ra::Engine::TextureManager::getInstance()->getOrLoadTexture( texParam );
        mat->addTexture( Ra::Engine::BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE, tex );
        // compute default weights uv
        showWeightsType( 0 );
    }
}

void SkinningComponent::skin() {
    CORE_ASSERT( m_isReady, "Skinning is not setup" );

    const Skeleton* skel = m_skeletonGetter();

    bool reset = ComponentMessenger::getInstance()->get<bool>( getEntity(), m_contentsName );

    // Reset the skin if it wasn't done before
    if ( reset && !m_frameData.m_doReset )
    {
        m_frameData.m_doReset      = true;
        m_frameData.m_frameCounter = 0;
    }
    else
    {
        m_frameData.m_currentPose = skel->getPose( SpaceType::MODEL );
        if ( !Ra::Core::Animation::areEqual( m_frameData.m_currentPose,
                                             m_frameData.m_previousPose ) ||
             m_forceUpdate )
        {
            m_forceUpdate            = false;
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
                Ra::Core::Animation::linearBlendSkinning( m_refData.m_referenceMesh.vertices(),
                                                          m_frameData.m_refToCurrentRelPose,
                                                          m_refData.m_weights,
                                                          m_frameData.m_currentPos );
                break;
            }
            case DQS:
            {
                Ra::Core::AlignedStdVector<DualQuaternion> DQ;
                Ra::Core::Animation::computeDQ(
                    m_frameData.m_refToCurrentRelPose, m_refData.m_weights, DQ );
                Ra::Core::Animation::dualQuaternionSkinning(
                    m_refData.m_referenceMesh.vertices(), DQ, m_frameData.m_currentPos );
                break;
            }
            case COR:
            {
                Ra::Core::Animation::corSkinning( m_refData.m_referenceMesh.vertices(),
                                                  m_frameData.m_refToCurrentRelPose,
                                                  m_refData.m_weights,
                                                  m_refData.m_CoR,
                                                  m_frameData.m_currentPos );
                break;
            }
            case STBS_LBS:
            {
                Ra::Core::Animation::linearBlendSkinningSTBS( m_refData.m_referenceMesh.vertices(),
                                                              m_frameData.m_refToCurrentRelPose,
                                                              *skel,
                                                              m_refData.m_skeleton,
                                                              m_refData.m_weights,
                                                              m_weightSTBS,
                                                              m_frameData.m_currentPos );
                break;
            }
            case STBS_DQS:
            {
                Ra::Core::AlignedStdVector<DualQuaternion> DQ;
                Ra::Core::Animation::computeDQSTBS( m_frameData.m_refToCurrentRelPose,
                                                    *skel,
                                                    m_refData.m_skeleton,
                                                    m_refData.m_weights,
                                                    m_weightSTBS,
                                                    DQ );
                Ra::Core::Animation::dualQuaternionSkinning(
                    m_refData.m_referenceMesh.vertices(), DQ, m_frameData.m_currentPos );
                break;
            }
            }
            Ra::Core::Animation::computeDQ(
                m_frameData.m_refToCurrentRelPose, m_refData.m_weights, m_DQ );
        }
    }
}

void uniformNormal( const Ra::Core::Vector3Array& p,
                    const Ra::Core::VectorArray<Ra::Core::Vector3ui>& T,
                    const std::vector<Ra::Core::Utils::Index>& duplicateTable,
                    Ra::Core::Vector3Array& normal ) {
    const uint N = p.size();
    normal.clear();
    normal.resize( N, Ra::Core::Vector3::Zero() );

    for ( const auto& t : T )
    {
        const Ra::Core::Utils::Index i = duplicateTable.at( t( 0 ) );
        const Ra::Core::Utils::Index j = duplicateTable.at( t( 1 ) );
        const Ra::Core::Utils::Index k = duplicateTable.at( t( 2 ) );
        const Ra::Core::Vector3 triN   = Ra::Core::Geometry::triangleNormal( p[i], p[j], p[k] );
        if ( !triN.allFinite() ) { continue; }
        normal[i] += triN;
        normal[j] += triN;
        normal[k] += triN;
    }

#pragma omp parallel for
    for ( int i = 0; i < N; ++i )
    {
        if ( !normal[i].isApprox( Ra::Core::Vector3::Zero() ) ) { normal[i].normalize(); }
    }

#pragma omp parallel for
    for ( int i = 0; i < N; ++i )
    {
        normal[i] = normal[duplicateTable[i]];
    }
}

void SkinningComponent::endSkinning() {
    if ( m_frameData.m_doSkinning )
    {
        Ra::Core::Vector3Array& vertices = *( m_verticesWriter() );
        Ra::Core::Vector3Array& normals  = *( m_normalsWriter() );

        vertices = m_frameData.m_currentPos;

        // FIXME: normals should be computed by the Skinning method!
        uniformNormal( vertices, m_refData.m_referenceMesh.m_triangles, m_duplicatesMap, normals );

        std::swap( m_frameData.m_previousPose, m_frameData.m_currentPose );
        std::swap( m_frameData.m_previousPos, m_frameData.m_currentPos );

        m_frameData.m_doSkinning = false;
    }
    else if ( m_frameData.m_doReset )
    {
        // Reset mesh to its initial state.
        Ra::Core::Vector3Array& vertices = *( m_verticesWriter() );
        Ra::Core::Vector3Array& normals  = *( m_normalsWriter() );

        vertices = m_refData.m_referenceMesh.vertices();
        normals  = m_refData.m_referenceMesh.normals();

        m_frameData.m_doReset       = false;
        m_frameData.m_currentPose   = m_refData.m_refPose;
        m_frameData.m_previousPose  = m_refData.m_refPose;
        m_frameData.m_currentPos    = m_refData.m_referenceMesh.vertices();
        m_frameData.m_previousPos   = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentNormal = m_refData.m_referenceMesh.normals();
        m_frameData.m_refToCurrentRelPose =
            Ra::Core::Animation::relativePose( m_frameData.m_currentPose, m_refData.m_refPose );
        m_frameData.m_prevToCurrentRelPose = Ra::Core::Animation::relativePose(
            m_frameData.m_currentPose, m_frameData.m_previousPose );
    }
}

void SkinningComponent::handleWeightsLoading( const Ra::Core::Asset::HandleData* data,
                                              const std::string& meshName ) {
    m_contentsName = data->getName();
    m_meshName     = meshName;
    setupIO( meshName );
    for ( const auto& bone : data->getComponentData() )
    {
        auto it = bone.m_weight.find( meshName );
        if ( it != bone.m_weight.end() ) { m_loadedWeights[bone.m_name] = it->second; }
    }
}

void SkinningComponent::createWeightMatrix() {
    m_refData.m_weights.resize( m_refData.m_referenceMesh.vertices().size(),
                                m_refData.m_skeleton.size() );
    for ( int col = 0; col < m_refData.m_skeleton.size(); ++col )
    {
        auto it = m_loadedWeights.find( m_refData.m_skeleton.getLabel( col ) );
        if ( it != m_loadedWeights.end() )
        {
            const auto& W   = it->second;
            const uint size = W.size();
            for ( uint i = 0; i < size; ++i )
            {
                const uint row                           = W[i].first;
                const Scalar w                           = W[i].second;
                m_refData.m_weights.coeffRef( row, col ) = w;
            }
        }
    }
    Ra::Core::Animation::checkWeightMatrix( m_refData.m_weights, false, true );

    if ( Ra::Core::Animation::normalizeWeights( m_refData.m_weights, true ) )
    { LOG( logINFO ) << "Skinning weights have been normalized"; }
}

void SkinningComponent::setupIO( const std::string& id ) {
    using DualQuatVector = Ra::Core::AlignedStdVector<Ra::Core::DualQuaternion>;

    ComponentMessenger::CallbackTypes<DualQuatVector>::Getter dqOut =
        std::bind( &SkinningComponent::getDQ, this );
    ComponentMessenger::getInstance()->registerOutput<DualQuatVector>(
        getEntity(), this, id, dqOut );

    ComponentMessenger::CallbackTypes<WeightMatrix>::Getter wOut =
        std::bind( &SkinningComponent::getWeightsOutput, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Animation::WeightMatrix>(
        getEntity(), this, id, wOut );

    ComponentMessenger::CallbackTypes<RefData>::Getter refData =
        std::bind( &SkinningComponent::getRefData, this );
    ComponentMessenger::getInstance()->registerOutput<Ra::Core::Skinning::RefData>(
        getEntity(), this, id, refData );

    ComponentMessenger::CallbackTypes<FrameData>::Getter frameData =
        std::bind( &SkinningComponent::getFrameData, this );
    ComponentMessenger::getInstance()->registerOutput<FrameData>(
        getEntity(), this, id, frameData );
}

void SkinningComponent::setSkinningType( SkinningType type ) {
    m_skinningType = type;
    if ( m_isReady )
    {
        setupSkinningType( type );
        m_forceUpdate = true;
    }
}

const Ra::Core::Animation::WeightMatrix* SkinningComponent::getWeightsOutput() const {
    return &m_refData.m_weights;
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
        if ( m_refData.m_CoR.empty() ) { Ra::Core::Animation::computeCoR( m_refData ); }
        break;
    }
    case STBS_DQS:
    {
        if ( m_DQ.empty() )
        {
            m_DQ.resize( m_refData.m_weights.rows(),
                         DualQuaternion( Quaternion( 0.0, 0.0, 0.0, 0.0 ),
                                         Quaternion( 0.0, 0.0, 0.0, 0.0 ) ) );
        }
    }
        [[fallthrough]];
    case STBS_LBS:
    {
        if ( m_weightSTBS.size() == 0 )
        {
            m_weightSTBS.resize( m_refData.m_weights.rows(), m_refData.m_weights.cols() );
            std::vector<Eigen::Triplet<Scalar>> triplets;
            const auto& V = m_refData.m_referenceMesh.vertices();
            for ( int i = 0; i < m_weightSTBS.rows(); ++i )
            {
                const auto& pi = V[i];
                for ( int j = 0; j < m_weightSTBS.cols(); ++j )
                {
                    Ra::Core::Vector3 a, b;
                    m_refData.m_skeleton.getBonePoints( j, a, b );
                    const Ra::Core::Vector3 ab = b - a;
                    Scalar t                   = Ra::Core::Geometry::projectOnSegment( pi, a, ab );
                    if ( t > 0 ) { triplets.push_back( Eigen::Triplet<Scalar>( i, j, t ) ); }
                }
            }
            m_weightSTBS.setFromTriplets( triplets.begin(), triplets.end() );
        }
    }
    } // end of switch.
}

void SkinningComponent::showWeights( bool on ) {
    m_showingWeights   = on;
    auto ro            = getRoMgr()->getRenderObject( *m_renderObjectReader() );
    TriangleMesh* mesh = const_cast<TriangleMesh*>( m_meshWritter() );
    auto attrUV        = Ra::Engine::Mesh::getAttribName( Ra::Engine::Mesh::VERTEX_TEXCOORD );
    Ra::Core::Utils::AttribHandle<Ra::Core::Vector3> handle;

    if ( m_showingWeights )
    {
        ro->setRenderTechnique( m_weightTechnique );
        // get the UV attrib handle, will create it if not there.
        handle                           = mesh->addAttrib<Ra::Core::Vector3>( attrUV );
        mesh->getAttrib( handle ).data() = m_weightsUV;
    }
    else
    {
        ro->setRenderTechnique( m_baseTechnique );
        handle = mesh->getAttribHandle<Ra::Core::Vector3>( attrUV );
        // if the UV attrib existed before, reset it, otherwise remove it.
        if ( m_baseUV.size() > 0 ) { mesh->getAttrib( handle ).data() = m_baseUV; }
        else
        {
            mesh->removeAttrib( handle );
            m_meshWritter(); // update the Engine::Mesh's handles
        }
    }
    m_forceUpdate = true;
}

void SkinningComponent::showWeightsType( int type ) {
    if ( !m_showingWeights ) return;
    const uint size = m_frameData.m_currentPos.size();
    m_weightsUV.resize( size, Ra::Core::Vector3::Zero() );
    m_weightType = type;
    if ( type == 0 )
    {
#pragma omp parallel for
        for ( int i = 0; i < int( size ); ++i )
        {
            m_weightsUV[i][0] = m_refData.m_weights.coeff( i, m_weightBone );
        }
    }
    else
    {
#pragma omp parallel for
        for ( int i = 0; i < int( size ); ++i )
        {
            m_weightsUV[i][0] = m_weightSTBS.coeff( i, m_weightBone );
        }
    }
    showWeights( true );
}

void SkinningComponent::setWeightBone( uint bone ) {
    if ( m_weightBone == bone ) return;
    m_weightBone = bone;
    showWeightsType( m_weightType );
}

} // namespace SkinningPlugin
