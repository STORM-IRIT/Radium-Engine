#ifndef RIGIDBODYPLUGIN_RIGIDBODYSYSTEM_HPP
#define RIGIDBODYPLUGIN_RIGIDBODYSYSTEM_HPP

#include <Plugins/RigidBodyPlugin/RigidBodyPlugin.hpp>

#include <Engine/Entity/System.hpp>

namespace Ra
{
    namespace Engine
    {
        class Component;
        class Entity;
        struct RenderTechnique;
    }
}

namespace RigidBodyPlugin
{
    class RigidBodyComponent;
}

namespace RigidBodyPlugin
{
    class RB_PLUGIN_API RigidBodySystem : public Ra::Engine::System
    {
    public:
        RigidBodySystem();
        virtual ~RigidBodySystem();

        virtual void initialize() override;

        virtual void handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder,
                                        const std::map<std::string, Ra::Core::Any>& data ) override;

        virtual Ra::Engine::Component* addComponentToEntity( Ra::Engine::Entity* entity ) override;

        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

        // TODO(Charly): Specialized factories with mass, etc
        RigidBodyComponent* addRigidBodyToEntity( Ra::Engine::Entity* entity, Scalar mass );
    };

} // namespace FancyMeshPlugin

#endif // RIGIDBODYPLUGIN_RIGIDBODYSYSTEM_HPP