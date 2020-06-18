#pragma once

#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Utils/Color.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/System/System.hpp>

/* This file contains a minimal radium/qt application which shows the
 * "Simple Animation" demo, which uses KeyFramedValues to spin a cube with
 * time-varying colors. */

namespace Ra::Engine {
class BlinnPhongMaterial;
} // namespace Ra::Engine

/// This is a very basic component which holds a spinning cube.
struct MinimalComponent : public Ra::Engine::Component {

    explicit MinimalComponent( Ra::Engine::Entity* entity );

    /// This function is called when the component is properly
    /// setup, i.e. it has an entity.
    void initialize() override;

    /// This function uses the keyframes to update the cube to time \p t.
    void update( Scalar t );

    /// The Keyframes for the cube's tranform.
    Ra::Core::Animation::KeyFramedValue<Ra::Core::Transform> m_transform;

    /// The Keyframes for the cube's color.
    Ra::Core::Animation::KeyFramedValue<Ra::Core::Utils::Color> m_color;

    /// The cube's material we change the color.
    std::shared_ptr<Ra::Engine::BlinnPhongMaterial> m_material;
};

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the update function of the component.
/// \note This system makes time loop around.
class MinimalSystem : public Ra::Engine::System
{
  public:
    virtual void generateTasks( Ra::Core::TaskQueue* q,
                                const Ra::Engine::FrameInfo& info ) override;
    void addComponent( Ra::Engine::Entity* ent, MinimalComponent* comp );
};
