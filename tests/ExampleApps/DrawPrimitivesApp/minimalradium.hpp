#pragma once

#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/System/System.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// This is a very basic component which holds a spinning cube.
struct MinimalComponent : public Ra::Engine::Component {

    explicit MinimalComponent( Ra::Engine::Entity* entity );

    /// This function is called when the component is properly
    /// setup, i.e. it has an entity.
    void initialize() override;
};

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
class MinimalSystem : public Ra::Engine::System
{
  public:
    virtual void generateTasks( Ra::Core::TaskQueue* q,
                                const Ra::Engine::FrameInfo& info ) override;
    void addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp );
};
