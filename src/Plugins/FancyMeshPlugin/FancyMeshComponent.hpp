#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <Plugins/FancyMeshPlugin/FancyMeshPlugin.hpp>

#include <Engine/Entity/Component.hpp>
#include <Plugins/FancyMeshPlugin/FancyMeshLoadingData.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
    }
}

namespace FancyMeshPlugin
{

    class FM_PLUGIN_API FancyMeshComponent : public Ra::Engine::Component
    {
    public:
        FancyMeshComponent( const std::string& name );
        virtual ~FancyMeshComponent();

        virtual void initialize() override;

        void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name );
        void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name,
                                  Ra::Engine::RenderTechnique* technique );

        void handleMeshLoading( const FancyComponentData& data );

    private:
        Ra::Core::Index m_renderObject;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
