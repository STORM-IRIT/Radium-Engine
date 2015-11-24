#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <Plugins/FancyMesh/FancyMeshPlugin.hpp>

#include <Engine/Entity/Component.hpp>
#include <Plugins/FancyMesh/FancyMeshLoadingData.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
    }
}

namespace FancyMeshPlugin
{
    struct MeshLoadingInfo
    {
        std::string filename;
        int index;
        std::vector<Ra::Core::VertexIdx> vertexMap;
    };

    class FM_PLUGIN_API FancyMeshComponent : public Ra::Engine::Component
    {
    public:
        FancyMeshComponent( const std::string& name );
        virtual ~FancyMeshComponent();

        virtual void initialize() override;

        virtual void rayCastQuery(const Ra::Core::Ray& r) const override;

        void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name );
        void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name,
                                  Ra::Engine::RenderTechnique* technique );

        void handleMeshLoading( const FancyComponentData& data );
        
        void setLoadingInfo(MeshLoadingInfo info);
        const MeshLoadingInfo &getLoadingInfo() const;
        
        Ra::Core::Index getMeshIndex() const;
        Ra::Core::TriangleMesh getMesh() const;
        
    private:
        MeshLoadingInfo m_loadingInfo;
        Ra::Core::Index m_meshIndex;
        Ra::Core::TriangleMesh m_mesh;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
