#ifndef RADIUMENGINE_POINTCLOUDSYSTEM_HPP
#define RADIUMENGINE_POINTCLOUDSYSTEM_HPP

#include <PointCloudPluginMacros.hpp>

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

namespace PointCloudPlugin
{
    class PointCloudComponent;
}

namespace PointCloudPlugin
{
    class PCLOUD_PLUGIN_API PointCloudSystem : public Ra::Engine::System
{
    public:
    PointCloudSystem();
    virtual ~PointCloudSystem();

        void setNeighSize(int size);
        void setUseNormal(bool useNormal);
        void setDepthThresh(double dThresh);
    virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

};

} // namespace PointCloudPlugin

#endif //RADIUMENGINE_POINTCLOUDSYSTEM_HPP
