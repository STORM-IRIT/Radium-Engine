#ifndef RIGIDBODYPLUGIN_RIGIDBODYSYSTEM_HPP
#define RIGIDBODYPLUGIN_RIGIDBODYSYSTEM_HPP

#include <Engine/Entity/System.hpp>

namespace Ra
{
    namespace Engine
    {
        class Component;
        class Entity;
        class RadiumEngine;
        struct RenderTechnique;
    }
}

namespace RigidBodyPlugin
{
    class RigidBodyComponent;
}

namespace RigidBodyPlugin
{
    class RA_API RigidBodySystem : public Ra::Engine::System
    {
    public:
        RigidBodySystem( Ra::Engine::RadiumEngine* engine );
        virtual ~RigidBodySystem();

        virtual void initialize() override;
        virtual void handleFileLoading( const std::string& filename ) override;

        virtual Ra::Engine::Component* addComponentToEntity( Ra::Engine::Entity* entity ) override;

        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

        // TODO(Charly): Specialized factories with mass, etc
        RigidBodyComponent* addRigidBodyToEntity( Ra::Engine::Entity* entity, Scalar mass );
    };

} // namespace FancyMeshPlugin

#endif // RIGIDBODYPLUGIN_RIGIDBODYSYSTEM_HPP