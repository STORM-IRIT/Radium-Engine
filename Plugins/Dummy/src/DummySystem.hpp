#ifndef DUMMYPLUGIN_DUMMYSYSTEM_HPP
#define DUMMYPLUGIN_DUMMYSYSTEM_HPP

#include <DummyPluginMacros.hpp>

#include <Engine/System/System.hpp>

namespace Ra
{
    namespace Core
    {
        struct TriangleMesh;
    }
}

namespace Ra
{
    namespace Engine
    {
        class Entity;
        struct RenderTechnique;
        class Component;
    }
}

namespace DummyPlugin
{
    class DummyComponent;

    struct DummyData
    {
        int foo;
        int bar;
    };

    class  DummySystem : public Ra::Engine::System
    {
    public:
        DummySystem();
        virtual ~DummySystem();

        virtual void initialize() override;
        virtual void handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder,
                                        const std::map<std::string, Ra::Core::Any>& data ) override;

        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

    protected:
        virtual Ra::Engine::Component* addComponentToEntityInternal(
                Ra::Engine::Entity* entity, uint id ) override;

    private:
        DummyData* m_data;
    };

} // namespace DummyPlugin

#endif // DUMMYPLUGIN_DUMMYSYSTEM_HPP
