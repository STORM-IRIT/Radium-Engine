#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <FancyMeshPlugin.hpp>

#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Component/Component.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
        class Mesh;
    }

    namespace Asset
    {
        class GeometryData;
    }
}

namespace FancyMeshPlugin
{
    class FM_PLUGIN_API FancyMeshComponent : public Ra::Engine::Component
    {
    public:
        FancyMeshComponent( const std::string& name, bool deformable = true );
        virtual ~FancyMeshComponent();


        virtual void initialize() override;
        virtual void rayCastQuery(const Ra::Core::Ray& r) const override;

        void addMeshRenderObject(const Ra::Core::TriangleMesh& mesh, const std::string& name);
        void handleMeshLoading(const Ra::Asset::GeometryData* data);

        /// Returns the index of the associated RO (the display mesh)
        Ra::Core::Index getRenderObjectIndex() const;

        /// Returns the current display geometry.
        const Ra::Core::TriangleMesh& getMesh() const;


    private:
        // Component communication management
        void setupIO(const std::string& id);

        const Ra::Engine::Mesh& getDisplayMesh() const;
        Ra::Engine::Mesh& getDisplayMesh();

        // Fancy mesh accepts to give its mesh and (if deformable) to update it
        const Ra::Core::TriangleMesh *getMeshOutput() const;
        void setMeshInput( const Ra::Core::TriangleMesh* mesh );
        Ra::Core::Vector3Array* getVerticesRw();
        Ra::Core::Vector3Array * getNormalsRw();
        Ra::Core::VectorArray<Ra::Core::Triangle>* getTrianglesRw();

    private:
        Ra::Core::Index m_meshIndex;
        Ra::Core::Index m_aabbIndex;
        std::string m_contentName;
        bool m_deformable;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
