#pragma once

#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/System.hpp>
#include <Engine/Scene/SystemDisplay.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// This is a very basic component which holds a spinning cube.
struct MinimalComponent : public Ra::Engine::Scene::Component {

    explicit MinimalComponent( Ra::Engine::Scene::Entity* entity );

    /// This function is called when the component is properly
    /// setup, i.e. it has an entity.
    void initialize() override;
};

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
class MinimalSystem : public Ra::Engine::Scene::System
{
  public:
    virtual void generateTasks( Ra::Core::TaskQueue* q,
                                const Ra::Engine::FrameInfo& info ) override;
    void addComponent( Ra::Engine::Scene::Entity* ent, MinimalComponent* comp );
};
