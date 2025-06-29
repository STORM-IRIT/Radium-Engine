﻿#include <Engine/Scene/SkinningComponent.hpp>

#include <Core/Animation/PoseOperation.hpp>

#include <Core/Animation/DualQuaternionSkinning.hpp>
#include <Core/Animation/HandleWeightOperation.hpp>
#include <Core/Animation/LinearBlendSkinning.hpp>
#include <Core/Animation/RotationCenterSkinning.hpp>
#include <Core/Geometry/DistanceQueries.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Log.hpp>

#include <Engine/Data/BlinnPhongMaterial.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

using namespace Ra::Core;

using Geometry::AttribArrayGeometry;
using Geometry::PolyMesh;
using Geometry::QuadMesh;
using Geometry::TriangleMesh;

using namespace Animation;
using SpaceType = HandleArray::SpaceType;

using namespace Utils;

namespace Ra {
namespace Engine {
namespace Scene {

static const std::string tangentName =
    Ra::Core::Geometry::getAttribName( Ra::Core::Geometry::VERTEX_TANGENT );
static const std::string bitangentName =
    Ra::Core::Geometry::getAttribName( Ra::Core::Geometry::VERTEX_BITANGENT );

TriangleMesh triangulate( const PolyMesh& polyMesh ) {
    TriangleMesh res;
    res.setVertices( polyMesh.vertices() );
    res.setNormals( polyMesh.normals() );
    res.copyAllAttributes( polyMesh );
    VectorArray<Vector3ui> indices;
    // using the same triangulation as in Ra::Engine::GeneralMesh::triangulate
    for ( const auto& face : polyMesh.getIndices() ) {
        if ( face.size() == 3 ) { indices.push_back( face ); }
        else {
            int minus { int( face.size() ) - 1 };
            int plus { 0 };
            while ( plus + 1 < minus ) {
                if ( ( plus - minus ) % 2 ) {
                    indices.emplace_back( face[plus], face[plus + 1], face[minus] );
                    ++plus;
                }
                else {
                    indices.emplace_back( face[minus], face[plus], face[minus - 1] );
                    --minus;
                }
            }
        }
    }
    res.setIndices( std::move( indices ) );
    return res;
}

TriangleMesh triangulate( const QuadMesh& quadMesh ) {
    TriangleMesh res;
    res.setVertices( quadMesh.vertices() );
    res.setNormals( quadMesh.normals() );
    res.copyAllAttributes( quadMesh );
    VectorArray<Vector3ui> indices;
    // using the same triangulation as in Ra::Engine::GeneralMesh::triangulate
    for ( const auto& face : quadMesh.getIndices() ) {
        indices.emplace_back( face[0], face[1], face[2] );
        indices.emplace_back( face[0], face[2], face[3] );
    }
    res.setIndices( std::move( indices ) );
    return res;
}

void SkinningComponent::initialize() {
    auto compMsg = ComponentMessenger::getInstance();
    // get the current animation data.
    bool hasSkel    = compMsg->canGet<Skeleton>( getEntity(), m_skelName );
    bool hasRefPose = compMsg->canGet<RefPose>( getEntity(), m_skelName );
    bool hasTriMesh = compMsg->canGet<TriangleMesh>( getEntity(), m_meshName );
    m_meshIsPoly    = compMsg->canGet<PolyMesh>( getEntity(), m_meshName );
    m_meshIsQuad    = compMsg->canGet<QuadMesh>( getEntity(), m_meshName );

    if ( hasSkel && hasRefPose && ( hasTriMesh || m_meshIsPoly || m_meshIsQuad ) ) {
        m_renderObjectReader = compMsg->getterCallback<Index>( getEntity(), m_meshName );
        m_skeletonGetter     = compMsg->getterCallback<Skeleton>( getEntity(), m_skelName );
        if ( hasTriMesh ) {
            m_triMeshWriter = compMsg->rwCallback<TriangleMesh>( getEntity(), m_meshName );
        }
        else if ( m_meshIsQuad ) {
            m_quadMeshWriter = compMsg->rwCallback<QuadMesh>( getEntity(), m_meshName );
        }
        else { m_polyMeshWriter = compMsg->rwCallback<PolyMesh>( getEntity(), m_meshName ); }

        // copy mesh triangles and find duplicates for normal computation.
        if ( hasTriMesh ) { m_refData.m_referenceMesh = *m_triMeshWriter(); }
        else if ( m_meshIsQuad ) { m_refData.m_referenceMesh = triangulate( *m_quadMeshWriter() ); }
        else { m_refData.m_referenceMesh = triangulate( *m_polyMeshWriter() ); }
        /// TODO : use the tangent computation algorithms from Core as soon as it is available.
        if ( !m_refData.m_referenceMesh.hasAttrib( tangentName ) &&
             !m_refData.m_referenceMesh.hasAttrib( bitangentName ) ) {
            const auto& normals = m_refData.m_referenceMesh.normals();
            Vector3Array tangents( normals.size() );
            Vector3Array bitangents( normals.size() );
#pragma omp parallel for
            for ( int i = 0; i < int( normals.size() ); ++i ) {
                Core::Math::getOrthogonalVectors( normals[i], tangents[i], bitangents[i] );
            }
            m_refData.m_referenceMesh.addAttrib( tangentName, std::move( tangents ) );
            m_refData.m_referenceMesh.addAttrib( bitangentName, std::move( bitangents ) );
        }
        else if ( !m_refData.m_referenceMesh.hasAttrib( tangentName ) ) {
            const auto& normals = m_refData.m_referenceMesh.normals();
            const auto& bH = m_refData.m_referenceMesh.getAttribHandle<Vector3>( bitangentName );
            const auto& bitangents = m_refData.m_referenceMesh.getAttrib( bH ).data();
            Vector3Array tangents( normals.size() );
#pragma omp parallel for
            for ( int i = 0; i < int( normals.size() ); ++i ) {
                tangents[i] = bitangents[i].cross( normals[i] );
            }
            m_refData.m_referenceMesh.addAttrib( tangentName, std::move( tangents ) );
        }
        else if ( !m_refData.m_referenceMesh.hasAttrib( bitangentName ) ) {
            const auto& normals = m_refData.m_referenceMesh.normals();
            const auto& tH      = m_refData.m_referenceMesh.getAttribHandle<Vector3>( tangentName );
            const auto& tangents = m_refData.m_referenceMesh.getAttrib( tH ).data();
            Vector3Array bitangents( normals.size() );
#pragma omp parallel for
            for ( int i = 0; i < int( normals.size() ); ++i ) {
                bitangents[i] = normals[i].cross( tangents[i] );
            }
            m_refData.m_referenceMesh.addAttrib( bitangentName, std::move( bitangents ) );
        }

        m_topoMesh = Ra::Core::Geometry::TopologicalMesh { m_refData.m_referenceMesh };

        auto ro = getRoMgr()->getRenderObject( *m_renderObjectReader() );
        // get other data
        m_refData.m_meshTransformInverse = ro->getLocalTransform().inverse();
        m_refData.m_skeleton             = *m_skeletonGetter();
        createWeightMatrix();

        // initialize frame data
        m_frameData.m_skeleton        = m_refData.m_skeleton;
        m_frameData.m_currentPosition = m_refData.m_referenceMesh.vertices();
        m_frameData.m_currentNormal   = m_refData.m_referenceMesh.normals();
        const auto& tH = m_refData.m_referenceMesh.getAttribHandle<Vector3>( tangentName );
        m_frameData.m_currentTangent = m_refData.m_referenceMesh.getAttrib( tH ).data();
        const auto& bH = m_refData.m_referenceMesh.getAttribHandle<Vector3>( bitangentName );
        m_frameData.m_currentBitangent = m_refData.m_referenceMesh.getAttrib( bH ).data();
        m_frameData.m_frameCounter     = 0;
        m_frameData.m_doSkinning       = true;
        m_frameData.m_doReset          = false;

        // setup comp data
        m_isReady     = true;
        m_forceUpdate = true;

        // prepare RO for skinning weights display
        m_baseMaterial = ro->getMaterial();

        // prepare UV
        auto attrUV = Ra::Core::Geometry::getAttribName( Ra::Core::Geometry::VERTEX_TEXCOORD );
        AttribArrayGeometry* geom;
        if ( hasTriMesh ) { geom = const_cast<TriangleMesh*>( m_triMeshWriter() ); }
        else {
            if ( m_meshIsPoly ) { geom = const_cast<PolyMesh*>( m_polyMeshWriter() ); }
            else { geom = const_cast<QuadMesh*>( m_quadMeshWriter() ); }
        }
        if ( geom->hasAttrib( attrUV ) ) {
            auto handle = geom->getAttribHandle<Vector3>( attrUV );
            m_baseUV    = geom->getAttrib( handle ).data();
        }

        // prepare weights material
        auto mat = new Data::BlinnPhongMaterial( "SkinningWeights_Mat" );
        mat->setDiffuseColor( Color::Skin() );
        mat->setSpecularColor( Color::White() );

        // assign texture
        Data::TextureParameters texParam;
        texParam.name = "Engine:Skinning:weights";
        mat->addTexture( Data::BlinnPhongMaterial::TextureSemantic::TEX_DIFFUSE, texParam );
        m_weightMaterial.reset( mat );

        // compute default weights uv
        showWeightsType( STANDARD );
    }
}

void SkinningComponent::skin() {
    CORE_ASSERT( m_isReady, "Skinning is not setup" );

    const Skeleton* skel = m_skeletonGetter();

    bool reset = ComponentMessenger::getInstance()->get<bool>( getEntity(), m_skelName );

    // Reset the skin if it wasn't done before
    if ( reset && !m_frameData.m_doReset ) {
        m_frameData.m_doReset      = true;
        m_frameData.m_frameCounter = 0;
        m_forceUpdate              = true;
    }
    const auto prevPose    = m_frameData.m_skeleton.getPose( SpaceType::MODEL );
    m_frameData.m_skeleton = *skel;
    auto currentPose       = m_frameData.m_skeleton.getPose( SpaceType::MODEL );
    if ( !areEqual( currentPose, prevPose ) || m_forceUpdate ) {
        m_forceUpdate            = false;
        m_frameData.m_doSkinning = true;
        m_frameData.m_frameCounter++;

        const auto tH = m_refData.m_referenceMesh.getAttribHandle<Vector3>( tangentName );
        const Vector3Array& tangents = m_refData.m_referenceMesh.getAttrib( tH ).data();
        const auto bH = m_refData.m_referenceMesh.getAttribHandle<Vector3>( bitangentName );
        const Vector3Array& bitangents = m_refData.m_referenceMesh.getAttrib( bH ).data();

        switch ( m_skinningType ) {
        case DQS: {
            dualQuaternionSkinning( m_refData, tangents, bitangents, m_frameData );
            break;
        }
        case COR: {
            centerOfRotationSkinning( m_refData, tangents, bitangents, m_frameData );
            break;
        }
        case LBS:
        default: {
            linearBlendSkinning( m_refData, tangents, bitangents, m_frameData );
            break;
        }
        }

        if ( m_normalSkinning == GEOMETRIC ) {
            m_topoMesh.updatePositions( m_frameData.m_currentPosition );
            m_topoMesh.updateWedgeNormals();
            m_topoMesh.updateTriangleMeshNormals( m_frameData.m_currentNormal );
#pragma omp parallel for
            for ( int i = 0; i < int( m_frameData.m_currentNormal.size() ); ++i ) {
                Core::Math::getOrthogonalVectors( m_frameData.m_currentNormal[i],
                                                  m_frameData.m_currentTangent[i],
                                                  m_frameData.m_currentBitangent[i] );
            }
        }
    }
}

void SkinningComponent::endSkinning() {
    if ( m_frameData.m_doSkinning ) {
        AttribArrayGeometry* geom;
        if ( !m_meshIsPoly ) {
            if ( !m_meshIsQuad ) { geom = const_cast<TriangleMesh*>( m_triMeshWriter() ); }
            else { geom = const_cast<QuadMesh*>( m_quadMeshWriter() ); }
        }
        else { geom = const_cast<PolyMesh*>( m_polyMeshWriter() ); }

        geom->setVertices( m_frameData.m_currentPosition );
        geom->setNormals( m_frameData.m_currentNormal );
        auto handle = geom->getAttribHandle<Vector3>( tangentName );
        if ( handle.idx().isValid() ) {
            geom->getAttrib( handle ).setData( m_frameData.m_currentTangent );
        }
        handle = geom->getAttribHandle<Vector3>( bitangentName );
        if ( handle.idx().isValid() ) {
            geom->getAttrib( handle ).setData( m_frameData.m_currentBitangent );
        }

        m_frameData.m_doReset    = false;
        m_frameData.m_doSkinning = false;
    }
}

void SkinningComponent::handleSkinDataLoading( const Asset::HandleData* data,
                                               const std::string& meshName ) {
    m_skelName = data->getName();
    m_meshName = meshName;
    setupIO( meshName );
    for ( const auto& bone : data->getComponentData() ) {
        auto it_w = bone.m_weights.find( meshName );
        if ( it_w != bone.m_weights.end() ) {
            m_loadedWeights[bone.m_name] = it_w->second;
            auto it_b                    = bone.m_bindMatrices.find( meshName );
            if ( it_b != bone.m_bindMatrices.end() ) {
                m_loadedBindMatrices[bone.m_name] = it_b->second;
            }
            else {
                LOG( logWARNING ) << "Bone " << bone.m_name
                                  << " has skinning weights but no bind matrix. Using Identity.";
                m_loadedBindMatrices[bone.m_name] = Transform::Identity();
            }
        }
    }
}

void SkinningComponent::createWeightMatrix() {
    m_refData.m_bindMatrices.resize( m_refData.m_skeleton.size(), Transform::Identity() );
    m_refData.m_weights.resize( int( m_refData.m_referenceMesh.vertices().size() ),
                                m_refData.m_skeleton.size() );
    std::vector<Eigen::Triplet<Scalar>> triplets;
    for ( uint col = 0; col < m_refData.m_skeleton.size(); ++col ) {
        std::string boneName = m_refData.m_skeleton.getLabel( col );
        auto it              = m_loadedWeights.find( boneName );
        if ( it != m_loadedWeights.end() ) {
            const auto& W = it->second;
            for ( uint i = 0; i < W.size(); ++i ) {
                const auto& w = W[i];
                int row { int( w.first ) };
                CORE_ASSERT( row < m_refData.m_weights.rows(),
                             "Weights are incompatible with mesh." );
                triplets.push_back( { row, int( col ), w.second } );
            }
            m_refData.m_bindMatrices[col] = m_loadedBindMatrices[boneName];
        }
    }
    m_refData.m_weights.setFromTriplets( triplets.begin(), triplets.end() );

    checkWeightMatrix( m_refData.m_weights, false, true );

    if ( normalizeWeights( m_refData.m_weights, true ) ) {
        LOG( logINFO ) << "Skinning weights have been normalized";
    }
}

void SkinningComponent::setupIO( const std::string& id ) {
    auto compMsg = ComponentMessenger::getInstance();

    auto refData = std::bind( &SkinningComponent::getSkinningRefData, this );
    compMsg->registerOutput<SkinningRefData>( getEntity(), this, id, refData );

    auto frameData = std::bind( &SkinningComponent::getSkinningFrameData, this );
    compMsg->registerOutput<SkinningFrameData>( getEntity(), this, id, frameData );
}

void SkinningComponent::setSkinningType( SkinningType type ) {
    m_skinningType = type;
    if ( m_isReady ) {
        // compute the per-vertex center of rotation only if required.
        // FIXME: takes time, would be nice to store them in a file and reload.
        if ( m_skinningType == COR && m_refData.m_CoR.empty() ) { computeCoR( m_refData ); }
        m_forceUpdate = true;
    }
}

void SkinningComponent::setNormalSkinning( NormalSkinning normalSkinning ) {
    m_normalSkinning = normalSkinning;
    if ( m_isReady ) { m_forceUpdate = true; }
}

const std::string& SkinningComponent::getMeshName() const {
    return m_meshName;
}

const std::string& SkinningComponent::getSkeletonName() const {
    return m_skelName;
}

void SkinningComponent::showWeights( bool on ) {
    m_showingWeights = on;
    auto ro          = getRoMgr()->getRenderObject( *m_renderObjectReader() );
    auto attrUV      = Ra::Core::Geometry::getAttribName( Ra::Core::Geometry::VERTEX_TEXCOORD );
    AttribHandle<Vector3> handle;

    AttribArrayGeometry* geom;
    if ( !m_meshIsPoly ) {
        if ( !m_meshIsQuad ) { geom = const_cast<TriangleMesh*>( m_triMeshWriter() ); }
        else { geom = const_cast<QuadMesh*>( m_quadMeshWriter() ); }
    }
    else { geom = const_cast<PolyMesh*>( m_polyMeshWriter() ); }

    if ( m_showingWeights ) {
        // update the displayed weights
        const auto size = m_frameData.m_currentPosition.size();
        m_weightsUV.resize( size, Vector3::Zero() );
        switch ( m_weightType ) {
        case STANDARD:
        default: {
#pragma omp parallel for
            for ( int i = 0; i < int( size ); ++i ) {
                m_weightsUV[i][0] = m_refData.m_weights.coeff( i, m_weightBone );
            }
        } break;
        } // end of switch.
        // change the material
        ro->setMaterial( m_weightMaterial );
        // TODO : as this considers renderer is ForwardRenderer, find how to generalize.
        ro->getRenderTechnique()->setParametersProvider( m_weightMaterial );
        // get the UV attrib handle, will create it if not there.
        handle = geom->addAttrib<Vector3>( attrUV );
        geom->getAttrib( handle ).setData( m_weightsUV );
    }
    else {
        // change the material
        ro->setMaterial( m_baseMaterial );
        ro->getRenderTechnique()->setParametersProvider( m_baseMaterial );
        // if the UV attrib existed before, reset it, otherwise remove it.
        handle = geom->getAttribHandle<Vector3>( attrUV );
        if ( m_baseUV.size() > 0 ) { geom->getAttrib( handle ).setData( m_baseUV ); }
        else { geom->removeAttrib( handle ); }
    }
    m_forceUpdate = true;
}

bool SkinningComponent::isShowingWeights() {
    return m_showingWeights;
}

void SkinningComponent::showWeightsType( WeightType type ) {
    m_weightType = type;
    if ( m_showingWeights ) { showWeights( true ); }
}

SkinningComponent::WeightType SkinningComponent::getWeightsType() {
    return m_weightType;
}

void SkinningComponent::setWeightBone( uint bone ) {
    m_weightBone = bone;
    if ( m_showingWeights ) { showWeights( true ); }
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
