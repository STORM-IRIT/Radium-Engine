
#include <minimalradium.hpp>

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Utils/Task.hpp>
#include <Core/Utils/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// This is a very basic component which holds a spinning cube.

MinimalComponent::MinimalComponent( Ra::Engine::Entity* entity ) :
    Ra::Engine::Component( "Minimal Component", entity ) {}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void MinimalComponent::initialize() {
    // Create a cube mesh render object.
    std::shared_ptr<Ra::Engine::Mesh> display( new Ra::Engine::Mesh( "Cube" ) );
    display->loadGeometry( Ra::Core::Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    auto renderObject = Ra::Engine::RenderObject::createRenderObject(
        "CubeRO", this, Ra::Engine::RenderObjectType::Fancy, display );
    addRenderObject( renderObject );
}

/// This function will spin our cube
void MinimalComponent::spin() {
    Ra::Core::Math::AngleAxis aa( 0.01f, Ra::Core::Math::Vector3::UnitY() );
    Ra::Core::Math::Transform rot( aa );

    auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
        m_renderObjects[0] );
    Ra::Core::Math::Transform t = ro->getLocalTransform();
    ro->setLocalTransform( rot * t );
}

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
void MinimalSystem::generateTasks( Ra::Core::Utils::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );

    // Create a new task which wil call c->spin() when executed.
    q->registerTask(
        new Ra::Core::Utils::FunctionTask( std::bind( &MinimalComponent::spin, c ), "spin" ) );
}
