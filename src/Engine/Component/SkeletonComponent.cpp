#include <Engine/Component/SkeletonComponent.hpp>

#include <fstream>
#include <iostream>
#include <queue>

#include <Core/Animation/KeyPose.hpp>
#include <Core/Animation/KeyTransform.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Asset/HandleToSkeleton.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Math/Math.hpp> // areApproxEqual

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

using Ra::Core::Animation::Animation;
using Ra::Core::Animation::HandleArray;
using Ra::Core::Animation::RefPose;
using Ra::Core::Animation::Skeleton;
using Ra::Core::Animation::WeightMatrix;
using Ra::Engine::ComponentMessenger;

using SpaceType = Ra::Core::Animation::HandleArray::SpaceType;

using namespace Ra::Core::Utils; // log

namespace Ra {
namespace Engine {

std::shared_ptr<Engine::Mesh> SkeletonComponent::s_boneMesh{nullptr};
std::shared_ptr<Engine::RenderTechnique> SkeletonComponent::s_boneRenderTechnique{nullptr};

SkeletonComponent::SkeletonComponent( const std::string& name, Ra::Engine::Entity* entity ) :
    Component( name, entity ) {}

SkeletonComponent::~SkeletonComponent() {}

// Component interface

bool SkeletonComponent::canEdit( const Ra::Core::Utils::Index& roIdx ) const {
    return (
        std::find_if( m_boneDrawables.begin(), m_boneDrawables.end(), [roIdx]( const auto& bone ) {
            return bone->getIndex() == roIdx;
        } ) != m_boneDrawables.end() );
}

Ra::Core::Transform SkeletonComponent::getTransform( const Ra::Core::Utils::Index& roIdx ) const {
    CORE_ASSERT( canEdit( roIdx ), "Transform is not editable" );
    const uint boneIdx = m_boneMap.at( roIdx );
    return m_skel.getPose( SpaceType::MODEL )[boneIdx];
}

// FIXME: we need the bones RO type
void SkeletonComponent::setTransform( const Ra::Core::Utils::Index& roIdx,
                                      const Ra::Core::Transform& transform ) {
    CORE_ASSERT( canEdit( roIdx ), "Transform is not editable" );
    // get bone data
    const uint boneIdx     = m_boneMap.at( roIdx );
    const auto& TBoneModel = m_skel.getTransform( boneIdx, SpaceType::MODEL );
    const auto& TBoneLocal = m_skel.getTransform( boneIdx, SpaceType::LOCAL );

    // turn bone translation into rotation for parent
    const uint pBoneIdx = m_skel.m_graph.parents()[boneIdx];
    if ( pBoneIdx != -1 && m_skel.m_graph.children()[pBoneIdx].size() == 1 )
    {
        const auto& pTBoneModel = m_skel.getTransform( pBoneIdx, SpaceType::MODEL );

        Ra::Core::Vector3 A;
        Ra::Core::Vector3 B;
        Ra::Core::Vector3 B_ = transform.translation();
        m_skel.getBonePoints( pBoneIdx, A, B );
        auto q = Ra::Core::Quaternion::FromTwoVectors( ( B - A ), ( B_ - A ) );
        Ra::Core::Transform R( q );
        R.pretranslate( A );
        R.translate( -A );
        m_skel.setTransform( pBoneIdx, R * pTBoneModel, SpaceType::MODEL );
    }

    // update bone local transform
    m_skel.setTransform( boneIdx, TBoneLocal * TBoneModel.inverse() * transform, SpaceType::LOCAL );
}

// Build from fileData

void SkeletonComponent::handleSkeletonLoading( const Ra::Core::Asset::HandleData* data ) {
    m_skelName = data->getName();

    m_skel.setName( data->getName() );

    Ra::Core::Asset::createSkeleton( *data, m_skel );

    m_refPose = m_skel.getPose( SpaceType::LOCAL );

    setupSkeletonDisplay();
    setupIO();
}

void SkeletonComponent::handleAnimationLoading(
    const std::vector<Ra::Core::Asset::AnimationData*>& data ) {
    m_animations.clear();
    m_animations.reserve( data.size() );

    for ( uint n = 0; n < data.size(); ++n )
    {
        std::map<uint, uint> table;
        std::set<Ra::Core::Animation::Time> keyTime;
        auto handleAnim = data[n]->getFrames();
        for ( uint i = 0; i < m_skel.size(); ++i )
        {
            for ( uint j = 0; j < handleAnim.size(); ++j )
            {
                if ( m_skel.getLabel( i ) == handleAnim[j].m_name )
                {
                    table[j] = i;
                    auto set = handleAnim[j].m_anim.timeSchedule();
                    keyTime.insert( set.begin(), set.end() );
                }
            }
        }

        if ( keyTime.empty() ) { continue; }

        Ra::Core::Animation::KeyPose keypose;
        Ra::Core::Animation::Pose pose = m_skel.m_pose;

        m_animations.push_back( Ra::Core::Animation::Animation() );
        for ( const auto& t : keyTime )
        {
            for ( const auto& it : table )
            {
                // pose[it.second] = ( m_skel.m_graph.isRoot( it.second ) ) ?
                // m_skel.m_pose[it.second] : handleAnim[it.first].m_anim.at( t );
                pose[it.second] = handleAnim[it.first].m_anim.at( t );
            }
            m_animations.back().addKeyPose( pose, t );
            keypose.insertKeyFrame( t, pose );
        }

        m_dt.push_back( data[n]->getTimeStep() );
    }
    if ( m_animations.size() == 0 )
    {
        m_animations.emplace_back();
        m_animations[0].addKeyPose( m_skel.getPose( SpaceType::LOCAL ), 0_ra );
    }
    m_animationID   = 0;
    m_animationTime = 0_ra;
}

// Skeleton-based animation data

void SkeletonComponent::setSkeleton( const Skeleton& skel ) {
    m_skel    = skel;
    m_refPose = skel.getPose( SpaceType::LOCAL );
    setupSkeletonDisplay();
}

SkeletonComponent::Animation& SkeletonComponent::addNewAnimation() {
    m_animations.emplace_back();
    m_animations.back().addKeyPose( m_skel.getPose( SpaceType::LOCAL ), 0_ra );
    return m_animations.back();
}

void SkeletonComponent::removeAnimation( const uint i ) {
    CORE_ASSERT( i < m_animations.size(), "Out of bound index." );
    m_animations.erase( m_animations.begin() + i );
    m_animationID = i > 1 ? i - 1 : 0;
}

void SkeletonComponent::useAnimation( const uint i ) {
    if ( i < m_animations.size() ) { m_animationID = i; }
}

size_t SkeletonComponent::getAnimationId() const {
    return m_animationID;
}

// Animation Process

void SkeletonComponent::update() {
    for ( auto& bone : m_boneDrawables )
    {
        uint boneIdx = m_boneMap.at( bone->getIndex() );
        Ra::Core::Vector3 start;
        Ra::Core::Vector3 end;
        m_skel.getBonePoints( boneIdx, start, end );

        Ra::Core::Transform scale = Ra::Core::Transform::Identity();
        scale.scale( ( end - start ).norm() );

        Ra::Core::Quaternion rot =
            Ra::Core::Quaternion::FromTwoVectors( Ra::Core::Vector3::UnitZ(), end - start );

        Ra::Core::Transform boneTransform = m_skel.getTransform( boneIdx, SpaceType::MODEL );
        Ra::Core::Matrix3 rotation        = rot.toRotationMatrix();
        Ra::Core::Transform drawTransform;
        drawTransform.linear()      = rotation;
        drawTransform.translation() = boneTransform.translation();

        bone->setLocalTransform( drawTransform * scale );
    }
}

void SkeletonComponent::goTo( Scalar t ) {
    m_wasReset = Ra::Core::Math::areApproxEqual( t, 0_ra );
    if ( m_wasReset )
    {
        m_animationTime = t;
        m_skel.setPose( m_refPose, SpaceType::LOCAL );
        return;
    }

    /// TODO: deal with animTimeStep
    //  // old code
    //    // Use the animation dt if required AND if we actually have animations.
    //    t = m_speed * ( ( m_animationTimeStep && m_dt.size() > 0 ) ? m_t[m_animationID] : t );
    //    // Ignore large dt that appear when the engine is paused (while loading a file for
    //    instance) if ( !m_animationTimeStep && ( t > 0.5_ra ) ) { t = 0_ra; }

    m_animationTime = m_speed * t;
    Scalar lastTime = m_animations[m_animationID].getDuration();
    if ( m_autoRepeat )
    {
        if ( !m_pingPong ) { m_animationTime = std::fmod( m_animationTime, lastTime ); }
        else
        {
            m_animationTime = std::fmod( m_animationTime, 2 * lastTime );
            if ( m_animationTime > lastTime ) { m_animationTime = 2 * lastTime - m_animationTime; }
        }
    }
    else if ( m_pingPong )
    {
        if ( m_animationTime > 2 * lastTime ) { m_animationTime = 0_ra; }
        else if ( m_animationTime > lastTime )
        { m_animationTime = 2 * lastTime - m_animationTime; }
    }

    // get the current pose from the animation
    Ra::Core::Animation::Pose pose = m_skel.getPose( SpaceType::LOCAL );
    if ( !m_animations.empty() ) { pose = m_animations[m_animationID].getPose( m_animationTime ); }
    else
    { pose = m_refPose; }
    m_skel.setPose( pose, SpaceType::LOCAL );
}

Scalar SkeletonComponent::getAnimationTime() const {
    return m_animationTime;
}

Scalar SkeletonComponent::getAnimationDuration() const {
    if ( m_animations.empty() ) { return 0_ra; }
    return m_animations[m_animationID].getDuration();
}

void SkeletonComponent::toggleAnimationTimeStep( const bool status ) {
    m_animationTimeStep = status;
}

bool SkeletonComponent::usesAnimationTimeStep() const {
    return m_animationTimeStep;
}

void SkeletonComponent::setSpeed( const Scalar value ) {
    m_speed = value;
}

Scalar SkeletonComponent::getSpeed() const {
    return m_speed;
}

void SkeletonComponent::autoRepeat( const bool status ) {
    m_autoRepeat = status;
}

bool SkeletonComponent::isAutoRepeat() const {
    return m_autoRepeat;
}

void SkeletonComponent::pingPong( const bool status ) {
    m_pingPong = status;
}

bool SkeletonComponent::isPingPong() const {
    return m_pingPong;
}

// Caching frames

void SkeletonComponent::cacheFrame( const std::string& dir, uint frame ) const {
    std::ofstream file( dir + "/" + m_skelName + "_frame" + std::to_string( frame ) + ".anim",
                        std::ios::trunc | std::ios::out | std::ios::binary );
    if ( !file.is_open() ) { return; }
    file.write( reinterpret_cast<const char*>( &m_animationID ), sizeof m_animationID );
    file.write( reinterpret_cast<const char*>( &m_animationTimeStep ), sizeof m_animationTimeStep );
    file.write( reinterpret_cast<const char*>( &m_animationTime ), sizeof m_animationTime );
    file.write( reinterpret_cast<const char*>( &m_speed ), sizeof m_speed );
    file.write( reinterpret_cast<const char*>( &m_autoRepeat ), sizeof m_autoRepeat );
    file.write( reinterpret_cast<const char*>( &m_pingPong ), sizeof m_pingPong );
    const auto& pose = m_skel.getPose( SpaceType::LOCAL );
    file.write( reinterpret_cast<const char*>( pose.data() ),
                ( sizeof pose[0] ) * uint( pose.size() ) );
    LOG( logINFO ) << "Saving anim data at time: " << m_animationTime;
}

bool SkeletonComponent::restoreFrame( const std::string& dir, uint frame ) {
    std::ifstream file( dir + "/" + m_skelName + "_frame" + std::to_string( frame ) + ".anim",
                        std::ios::in | std::ios::binary );
    if ( !file.is_open() ) { return false; }
    if ( !file.read( reinterpret_cast<char*>( &m_animationID ), sizeof m_animationID ) )
    { return false; }
    if ( !file.read( reinterpret_cast<char*>( &m_animationTimeStep ), sizeof m_animationTimeStep ) )
    { return false; }
    if ( !file.read( reinterpret_cast<char*>( &m_animationTime ), sizeof m_animationTime ) )
    { return false; }
    if ( !file.read( reinterpret_cast<char*>( &m_speed ), sizeof m_speed ) ) { return false; }
    if ( !file.read( reinterpret_cast<char*>( &m_autoRepeat ), sizeof m_autoRepeat ) )
    { return false; }
    if ( !file.read( reinterpret_cast<char*>( &m_pingPong ), sizeof m_pingPong ) ) { return false; }
    auto pose = m_skel.getPose( SpaceType::LOCAL );
    if ( !file.read( reinterpret_cast<char*>( pose.data() ),
                     ( sizeof pose[0] ) * uint( pose.size() ) ) )
    { return false; }
    m_skel.setPose( pose, SpaceType::LOCAL );

    // update the render objects
    update();

    return true;
}

// Skeleton display

void SkeletonComponent::setXray( bool on ) const {
    for ( auto& b : m_boneDrawables )
    {
        b->setXRay( on );
    }
}

bool SkeletonComponent::isXray() const {
    if ( m_boneDrawables.size() == 0 ) return false;
    return m_boneDrawables.front()->isXRay();
}

void SkeletonComponent::toggleSkeleton( const bool status ) {
    for ( const auto& b : m_boneDrawables )
    {
        b->setVisible( status );
    }
}

bool SkeletonComponent::isShowingSkeleton() const {
    if ( m_boneDrawables.size() == 0 ) return false;
    return m_boneDrawables.front()->isVisible();
}

Ra::Core::Geometry::TriangleMesh makeBoneShape() {
    // Bone along Z axis.
    Ra::Core::Geometry::TriangleMesh mesh;
    const Scalar l = 0.1_ra;
    const Scalar w = 0.1_ra;
    mesh.setVertices( {Ra::Core::Vector3( 0, 0, 0 ),
                       Ra::Core::Vector3( 0, 0, 1 ),
                       Ra::Core::Vector3( 0, w, l ),
                       Ra::Core::Vector3( w, 0, l ),
                       Ra::Core::Vector3( 0, -w, l ),
                       Ra::Core::Vector3( -w, 0, l )} );

    mesh.setNormals( {Ra::Core::Vector3( 0, 0, -1 ),
                      Ra::Core::Vector3( 0, 0, 1 ),
                      Ra::Core::Vector3( 0, 1, 0 ),
                      Ra::Core::Vector3( 1, 0, 0 ),
                      Ra::Core::Vector3( 0, -1, 0 ),
                      Ra::Core::Vector3( -1, 0, 0 )} );

    mesh.m_indices = {Ra::Core::Vector3ui( 0, 2, 3 ),
                      Ra::Core::Vector3ui( 0, 5, 2 ),
                      Ra::Core::Vector3ui( 0, 3, 4 ),
                      Ra::Core::Vector3ui( 0, 4, 5 ),
                      Ra::Core::Vector3ui( 1, 3, 2 ),
                      Ra::Core::Vector3ui( 1, 2, 5 ),
                      Ra::Core::Vector3ui( 1, 4, 3 ),
                      Ra::Core::Vector3ui( 1, 5, 4 )};
    return mesh;
}

void SkeletonComponent::setupSkeletonDisplay() {
    m_renderObjects.clear();
    m_boneDrawables.clear();
    if ( !s_boneMesh )
    {
        s_boneMesh = std::make_shared<Ra::Engine::Mesh>( "Bone Mesh" );
        s_boneMesh->loadGeometry( makeBoneShape() );
        auto mat  = Ra::Core::make_shared<Ra::Engine::BlinnPhongMaterial>( "Bone Material" );
        mat->m_kd = Ra::Core::Utils::Color::Grey( 0.4_ra );
        mat->m_ks = Ra::Core::Utils::Color::Black();
        s_boneRenderTechnique.reset( new Ra::Engine::RenderTechnique );
        s_boneRenderTechnique->setConfiguration(
            *Ra::Engine::ShaderConfigurationFactory::getConfiguration( "BlinnPhong" ) );
        s_boneRenderTechnique->setParametersProvider( mat );
    }
    auto mat = const_cast<Ra::Engine::BlinnPhongMaterial*>(
        static_cast<const Ra::Engine::BlinnPhongMaterial*>(
            s_boneRenderTechnique->getParametersProvider() ) );
    for ( uint i = 0; i < m_skel.size(); ++i )
    {
        if ( !m_skel.m_graph.isLeaf( i ) && !m_skel.m_graph.isRoot( i ) &&
             m_skel.getLabel( i ).find( "_$AssimpFbx$_" ) == std::string::npos )
        {
            std::string name = m_skel.getLabel( i ) + "_" + std::to_string( i );
            auto ro          = Ra::Engine::RenderObject::createRenderObject(
                name,
                this,
                Ra::Engine::RenderObjectType::Geometry,
                s_boneMesh,
                *s_boneRenderTechnique );
            ro->setXRay( false );
            ro->setMaterial( std::shared_ptr<Ra::Engine::BlinnPhongMaterial>( mat ) );
            addRenderObject( ro );
            m_boneMap[m_renderObjects.back()] = i;
            m_boneDrawables.push_back( ro );
        }
        else
        { LOG( logDEBUG ) << "Bone " << m_skel.getLabel( i ) << " not displayed."; }
    }
    update();
}

void SkeletonComponent::printSkeleton( const Skeleton& skeleton ) {
    std::deque<uint> queue;
    std::deque<int> levels;

    queue.push_back( 0 );
    levels.push_back( 0 );
    while ( !queue.empty() )
    {
        uint i = queue.front();
        queue.pop_front();
        int level = levels.front();
        levels.pop_front();
        std::cout << i << " " << skeleton.getLabel( i ) << "\t";
        for ( const auto& c : skeleton.m_graph.children()[i] )
        {
            queue.push_back( c );
            levels.push_back( level + 1 );
        }

        if ( levels.front() != level ) { std::cout << std::endl; }
    }
}

// Component Communication (CC)

void SkeletonComponent::setupIO() {
    const auto& cm = ComponentMessenger::getInstance();

    auto skelOut = std::bind( &SkeletonComponent::getSkeletonOutput, this );
    cm->registerOutput<Skeleton>( getEntity(), this, m_skelName, skelOut );

    using BoneMap   = std::map<Ra::Core::Utils::Index, uint>;
    auto boneMapOut = std::bind( &SkeletonComponent::getBoneRO2idx, this );
    cm->registerOutput<BoneMap>( getEntity(), this, m_skelName, boneMapOut );

    auto refpOut = std::bind( &SkeletonComponent::getRefPoseOutput, this );
    cm->registerOutput<Ra::Core::Animation::Pose>( getEntity(), this, m_skelName, refpOut );

    auto animOut = std::bind( &SkeletonComponent::getAnimationOutput, this );
    cm->registerOutput<Animation>( getEntity(), this, m_skelName, animOut );

    auto timeOut = std::bind( &SkeletonComponent::getTimeOutput, this );
    cm->registerOutput<Scalar>( getEntity(), this, m_skelName, timeOut );

    auto resetOut = std::bind( &SkeletonComponent::getWasReset, this );
    cm->registerOutput<bool>( getEntity(), this, m_skelName, resetOut );
}

const Ra::Core::Animation::Skeleton* SkeletonComponent::getSkeletonOutput() const {
    return &m_skel;
}

const std::map<Ra::Core::Utils::Index, uint>* SkeletonComponent::getBoneRO2idx() const {
    return &m_boneMap;
}

const Ra::Core::Animation::RefPose* SkeletonComponent::getRefPoseOutput() const {
    return &m_refPose;
}

const SkeletonComponent::Animation* SkeletonComponent::getAnimationOutput() const {
    if ( m_animations.empty() ) { return nullptr; }
    return &m_animations[m_animationID];
}

const Scalar* SkeletonComponent::getTimeOutput() const {
    return &m_animationTime;
}

const bool* SkeletonComponent::getWasReset() const {
    return &m_wasReset;
}

} // namespace Engine
} // namespace Ra
