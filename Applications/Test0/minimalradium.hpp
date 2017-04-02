#pragma once

#include <Engine/RadiumEngine.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Entity/Entity.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

/// This is a very basic component which holds a spinning cube.
    struct MinimalComponent : public Ra::Engine::Component {

        MinimalComponent();

        /// This function is called when the component is properly
        /// setup, i.e. it has an entity.
        void initialize() override;

        /// This function will spin our cube
        void spin();
    };

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
    class MinimalSystem : public Ra::Engine::System {
    public:
        virtual void generateTasks(Ra::Core::TaskQueue *q, const Ra::Engine::FrameInfo &info) override;
    };
