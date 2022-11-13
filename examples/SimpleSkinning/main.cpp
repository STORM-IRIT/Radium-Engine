#include <Core/Animation/Skeleton.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Types.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/CameraManager.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/SkeletonBasedAnimationSystem.hpp>
#include <Engine/Scene/SkeletonComponent.hpp>
#include <Engine/Scene/SkinningComponent.hpp>
#include <Engine/Scene/System.hpp>
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Viewer/Viewer.hpp>

using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Core::Animation;
using namespace Ra::Core::Math;
using namespace Ra::Engine;
using namespace Ra::Engine::Scene;

class EntityAnimationSystem : public Scene::System
{
  public:
    void setSkeleton( SkeletonComponent* skel ) { m_skel = skel; }
    void setSkin( SkinningComponent* skin ) { m_skin = skin; }

    void generateTasks( TaskQueue* q, const FrameInfo& info ) override {

        if ( !m_skel || !m_skin ) return;

        m_t = info.m_animationTime;

        auto animTaskId = q->registerTask( new Ra::Core::FunctionTask(
            [this]() {
                Transform transform;
                transform =
                    Transform( AngleAxis( cos( m_t + Math::Pi ), Vector3( 1_ra, 0_ra, 0_ra ) ) *
                               Translation( Vector3( 0_ra, 0_ra, 5_ra ) ) );

                m_skel->setBoneTransform( 2, transform, Skeleton::SpaceType::LOCAL );
                transform = Transform( AngleAxis( cos( m_t ), Vector3( 1_ra, 0_ra, 0_ra ) ) *
                                       Translation( Vector3( 0_ra, 0_ra, 5_ra ) ) );

                m_skel->setBoneTransform( 3, transform, Skeleton::SpaceType::LOCAL );
            },
            "AnimTask_" + m_skin->getMeshName() ) );

        auto skinFunc = std::bind( &SkinningComponent::skin, m_skin );
        auto skinTask = new Core::FunctionTask( skinFunc, "SkinnerTask_" + m_skin->getMeshName() );
        auto endFunc  = std::bind( &SkinningComponent::endSkinning, m_skin );
        auto endTask = new Core::FunctionTask( endFunc, "SkinnerEndTask_" + m_skin->getMeshName() );

        auto skinTaskId = q->registerTask( skinTask );
        auto endTaskId  = q->registerTask( endTask );
        q->addDependency( animTaskId, skinTaskId );
        q->addDependency( skinTaskId, endTaskId );
    }

  private:
    Scalar m_t { 0_ra };
    SkeletonComponent* m_skel { nullptr };
    SkinningComponent* m_skin { nullptr };
};

void setupScene( Ra::Engine::RadiumEngine* engine ) {
    auto animationSystem = new EntityAnimationSystem;
    engine->registerSystem( "Simple animation system", animationSystem );

    auto entity = engine->getEntityManager()->createEntity( "Cylinder" );

    auto cylinder =
        Geometry::makeSharpCylinder( { 0_ra, 0_ra, 0_ra }, { 0_ra, 0_ra, 10_ra }, 2_ra, 16, 32 );

    // component ownership is transfered to entity in component ctor
    auto meshComponent =
        new TriangleMeshComponent( "Cylinder", entity, std::move( cylinder ), nullptr );

    std::map<std::string, Core::Transform> boneMatrices;
    std::map<std::string, std::vector<std::pair<uint, Scalar>>> boneWeights;

    auto skel            = Ra::Core::Animation::Skeleton();
    auto transform       = Transform::Identity();
    auto rootId          = skel.addRoot( transform, "root" );
    boneMatrices["root"] = skel.getTransform( rootId, Skeleton::SpaceType::MODEL );

    transform         = Transform( Translation( Vector3( 0_ra, 0_ra, 0_ra ) ) );
    auto id1          = skel.addBone( rootId, transform, Skeleton::SpaceType::MODEL, "1" );
    boneMatrices["1"] = skel.getTransform( id1, Skeleton::SpaceType::MODEL ).inverse();
    std::vector<std::pair<uint, Scalar>> w;
    auto& vertices = meshComponent->getCoreGeometry().vertices();

    transform         = Transform( Translation( Vector3( 0_ra, 0_ra, 5_ra ) ) );
    auto id2          = skel.addBone( id1, transform, Skeleton::SpaceType::MODEL, "2" );
    boneMatrices["2"] = skel.getTransform( id2, Skeleton::SpaceType::MODEL ).inverse();
    for ( int i = 0; i < vertices.size(); ++i ) {
        w.emplace_back( i, 1_ra - smoothstep( 0.3_ra, 0.7_ra, vertices[i].z() / 10_ra ) );
    }
    boneWeights["2"] = w;

    transform         = Transform( Translation( Vector3( 0_ra, 0_ra, 10_ra ) ) );
    auto id3          = skel.addBone( id2, transform, Skeleton::SpaceType::MODEL, "3" );
    boneMatrices["3"] = skel.getTransform( id3, Skeleton::SpaceType::MODEL ).inverse();
    for ( auto& p : w ) {
        p.second = 1_ra - p.second;
    }
    boneWeights["3"] = w;

    auto skeletonComponent = new SkeletonComponent( "AC_Cylinder", entity );
    skeletonComponent->setSkeleton( skel );
    skeletonComponent->initialize();
    skeletonComponent->setXray( true );
    skeletonComponent->toggleSkeleton( true );

    auto skinningComponent =
        new SkinningComponent( "SK_Cylinder", SkinningComponent::SkinningType::LBS, entity );
    skinningComponent->setMeshName( meshComponent->getName() );
    skinningComponent->setPerBoneMatrix( std::move( boneMatrices ) );
    skinningComponent->setPerBoneWeight( std::move( boneWeights ) );
    skinningComponent->initialize();

    // add these two to our animationSystem
    animationSystem->setSkeleton( skeletonComponent );
    animationSystem->setSkin( skinningComponent );
}

int main( int argc, char* argv[] ) {
    Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );

    app.m_engine->setEndTime( std::numeric_limits<Scalar>::max() );
    app.m_engine->setRealTime( true );
    app.m_engine->play( true );

    setupScene( app.m_engine );

    app.m_mainWindow->prepareDisplay();

    app.m_mainWindow->getViewer()->makeCurrent();
    app.m_mainWindow->getViewer()->getRenderer()->buildAllRenderTechniques();
    app.m_mainWindow->getViewer()->doneCurrent();

    return app.exec();
}
