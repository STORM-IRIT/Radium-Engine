
#include <minimalradium.hpp>

#include <Core/Animation/KeyFramedValueInterpolators.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>

#include <Engine/FrameInfo.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

/* This file contains a minimal radium/qt application which shows the
 * "Simple Animation" demo, which uses KeyFramedValues to spin a cube with
 * time-varying colors. */

/// This is a very basic component which holds a spinning cube.

MinimalComponent::MinimalComponent( Ra::Engine::Entity* entity ) :
    Ra::Engine::Component( "Minimal Component", entity ),
    m_transform( Ra::Core::Transform::Identity(), 0_ra ),
    m_color( Ra::Core::Utils::Color::Green(), 0_ra ) {
    //! [Creating the transform KeyFrames]
    Ra::Core::Transform T = Ra::Core::Transform::Identity();
    T.rotate( Ra::Core::AngleAxis( Ra::Core::Math::PiDiv3, Ra::Core::Vector3::UnitY() ) );
    m_transform.insertKeyFrame( 1_ra, T );
    T.rotate( Ra::Core::AngleAxis( Ra::Core::Math::PiDiv3, Ra::Core::Vector3::UnitY() ) );
    m_transform.insertKeyFrame( 2_ra, T );
    T.rotate( Ra::Core::AngleAxis( Ra::Core::Math::PiDiv3, Ra::Core::Vector3::UnitY() ) );
    m_transform.insertKeyFrame( 3_ra, T );
    //! [Creating the transform KeyFrames]
    m_color.insertKeyFrame( 1_ra, Ra::Core::Utils::Color::Red() );
    m_color.insertKeyFrame( 2_ra, Ra::Core::Utils::Color::Blue() );
    m_color.insertKeyFrame( 3_ra, Ra::Core::Utils::Color::Green() );
    //! [Creating the color KeyFrames]
}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void MinimalComponent::initialize() {
    //! [Creating the Engine Mesh]
    // Create the Engine mesh
    std::shared_ptr<Ra::Engine::Mesh> theMesh( new Ra::Engine::Mesh( "Cube" ) );
    // Set the core geometry of the engine mesh
    theMesh->loadGeometry( Ra::Core::Geometry::makeSharpBox( {0.1_ra, 0.1_ra, 0.1_ra} ) );
    //! [Creating the Engine Mesh]
    //! [Creating the Material]
    // Create the desired material - here a Ra::Engine::BlinnPhongMaterial
    m_material = Ra::Core::make_shared<Ra::Engine::BlinnPhongMaterial>( "theCubeMaterial" );
    // Set the material parameters
    m_material->m_kd = Ra::Core::Utils::Color::Green();
    m_material->m_ks = Ra::Core::Utils::Color::White();
    m_material->m_ns = 64_ra;
    //! [Creating the Material]
    //! [Creating the RenderTechnique]
    // Create the render technique
    Ra::Engine::RenderTechnique theRenderTechnique;
    // Get the default rendertechnique configurator for the material
    auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
    // Configure the render technique for an opaque material (false arg)
    builder.second( theRenderTechnique, false );
    // Associate the material as parameter proviedr for rendering
    theRenderTechnique.setParametersProvider( m_material );
    //! [Creating the RenderTechnique]
    //! [Creating the RenderObject]
    // Create the renderObject
    auto theRenderObject = Ra::Engine::RenderObject::createRenderObject(
        "CubeRO", this, Ra::Engine::RenderObjectType::Geometry, theMesh, theRenderTechnique );
    // Add the renderObject to the component
    addRenderObject( theRenderObject );
    //! [Creating the RenderObject]
}

/// This function will spin our cube
void MinimalComponent::update( Scalar t ) {
    auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
        m_renderObjects[0] );
    //! [Fetch transform from KeyFramedValue]
    auto T = m_transform.at( t, Ra::Core::Animation::linearInterpolate<Ra::Core::Transform> );
    //! [Fetch transform from KeyFramedValue]
    ro->setLocalTransform( T );

    //! [Fetch color from KeyFramedValue]
    auto C = m_color.at( t, Ra::Core::Animation::linearInterpolate<Ra::Core::Utils::Color> );
    //! [Fetch color from KeyFramedValue]
    m_material->m_kd = C;
    m_material->needUpdate();
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );

    // compute the current animation time
    static Scalar time = 0_ra;
    auto t0            = c->m_transform.getKeyFrames().begin()->first;
    auto t1            = c->m_transform.getKeyFrames().rbegin()->first;
    auto t             = time + info.m_dt;
    if ( t > t1 ) { t = t0 + t - t1; }
    time = t;

    // Create a new task which wil call c->spin() when executed.
    q->registerTask(
        new Ra::Core::FunctionTask( std::bind( &MinimalComponent::update, c, time ), "spin" ) );
}

void MinimalSystem::addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
}
