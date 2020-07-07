
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
    m_transform( Ra::Core::Transform::Identity(), 0_ra ) {
    //! [Creating the transform KeyFrames]
    Ra::Core::Transform T = Ra::Core::Transform::Identity();
    T.rotate( Ra::Core::AngleAxis( Ra::Core::Math::PiDiv3, Ra::Core::Vector3::UnitY() ) );
    m_transform.insertKeyFrame( 1_ra, T );
    T.rotate( Ra::Core::AngleAxis( Ra::Core::Math::PiDiv3, Ra::Core::Vector3::UnitY() ) );
    m_transform.insertKeyFrame( 2_ra, T );
    T.rotate( Ra::Core::AngleAxis( Ra::Core::Math::PiDiv3, Ra::Core::Vector3::UnitY() ) );
    m_transform.insertKeyFrame( 3_ra, T );
    //! [Creating the transform KeyFrames]
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
    auto material = Ra::Core::make_shared<Ra::Engine::BlinnPhongMaterial>( "theCubeMaterial" );
    // Set the material parameters
    material->m_kd = Ra::Core::Utils::Color::Green();
    material->m_ks = Ra::Core::Utils::Color::White();
    material->m_ns = 64_ra;
    //! [Creating the Material]
    //! [Creating the RenderTechnique]
    // Create the render technique
    Ra::Engine::RenderTechnique theRenderTechnique;
    // Get the default rendertechnique configurator for the material
    auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "BlinnPhong" );
    // Configure the render technique for an opaque material (false arg)
    builder.second( theRenderTechnique, false );
    // Associate the material as parameter proviedr for rendering
    theRenderTechnique.setParametersProvider( material );
    //! [Creating the RenderTechnique]
    //! [Creating the RenderObject]
    // Create the renderObject
    auto theRenderObject = Ra::Engine::RenderObject::createRenderObject(
        "CubeRO", this, Ra::Engine::RenderObjectType::Geometry, theMesh, theRenderTechnique );
    // Add the renderObject to the component
    addRenderObject( theRenderObject );
    //! [Creating the RenderObject]
    //! [Creating the color KeyFrames]
    auto colors = new Ra::Core::Animation::KeyFramedValue<Ra::Core::Utils::Color>(
        Ra::Core::Utils::Color::Green(), 0_ra );
    colors->insertKeyFrame( 1_ra, Ra::Core::Utils::Color::Red() );
    colors->insertKeyFrame( 2_ra, Ra::Core::Utils::Color::Blue() );
    colors->insertKeyFrame( 3_ra, Ra::Core::Utils::Color::Green() );
    //! [Attach the color KeyFrames to a controller]
    m_colorController.m_value   = colors;
    m_colorController.m_updater = [colors, material]( const Scalar& t ) {
        auto C = colors->at( t, Ra::Core::Animation::linearInterpolate<Ra::Core::Utils::Color> );
        material->m_kd = C;
        material->needUpdate();
    };
    //! [Attach the color KeyFrames to a controller]
    //! [Creating the color KeyFrames]
}

/// This function will spin our cube
void MinimalComponent::update( Scalar t ) {
    auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
        m_renderObjects[0] );
    //! [Fetch transform from KeyFramedValue]
    auto T = m_transform.at( t, Ra::Core::Animation::linearInterpolate<Ra::Core::Transform> );
    //! [Fetch transform from KeyFramedValue]
    ro->setLocalTransform( T );

    //! [Update color from KeyFramedValue]
    m_colorController.updateKeyFrame( t );
    //! [Update color from KeyFramedValue]
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );

    // Create a new task which wil call c->spin() when executed.
    q->registerTask( new Ra::Core::FunctionTask(
        std::bind( &MinimalComponent::update, c, info.m_animationTime ), "spin" ) );
}

void MinimalSystem::addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp ) {
    registerComponent( ent, comp );
}
