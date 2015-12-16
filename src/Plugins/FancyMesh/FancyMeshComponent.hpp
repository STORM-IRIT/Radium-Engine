#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <Plugins/FancyMesh/FancyMeshPlugin.hpp>

#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Entity/Component.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
    }

    namespace Asset
    {
        class FileData;
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

        virtual void rayCastQuery(const Ra::Core::Ray& r) const override;

        void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name );
        void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name,
                                  Ra::Engine::RenderTechnique* technique );

        void handleMeshLoading( const Ra::Asset::FileData* fileData );
        
        Ra::Core::Index getMeshIndex() const;
        Ra::Core::TriangleMesh getMesh() const;
        
    private:
        Ra::Core::Index m_meshIndex;
        Ra::Core::TriangleMesh m_mesh;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
