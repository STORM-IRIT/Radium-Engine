#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <FancyMeshPluginMacros.hpp>

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
    /*!
     * \brief The FancyMeshComponent class
     *
     * Exports access to the mesh geometry:
     *  - TriangleMesh: get, rw (set vertices, normals and triangles dirty)
     *  - Vertices: rw (if deformable)
     *  - normals: rw (if deformable)
     *  - triangles: rw (if deformable)
     */
    class FM_PLUGIN_API FancyMeshComponent : public Ra::Engine::Component
    {
    public:
        FancyMeshComponent( const std::string& name, bool deformable = true );
        virtual ~FancyMeshComponent();


        virtual void initialize() override;

        void addMeshRenderObject(const Ra::Core::TriangleMesh& mesh, const std::string& name);
        void handleMeshLoading(const Ra::Asset::GeometryData* data);

        /// Returns the index of the associated RO (the display mesh)
        Ra::Core::Index getRenderObjectIndex() const;

        /// Returns the current display geometry.
        const Ra::Core::TriangleMesh& getMesh() const;


    public:
        // Component communication management
        void setupIO(const std::string& id);
        void setContentName (const std::string name);
        void setDeformable (const bool b);
    private:
        const Ra::Engine::Mesh& getDisplayMesh() const;
        Ra::Engine::Mesh& getDisplayMesh();

        // Fancy mesh accepts to give its mesh and (if deformable) to update it
        const Ra::Core::TriangleMesh* getMeshOutput() const;
        Ra::Core::TriangleMesh* getMeshRw();
        void setMeshInput( const Ra::Core::TriangleMesh* mesh );
        Ra::Core::Vector3Array* getVerticesRw();
        Ra::Core::Vector3Array* getNormalsRw();
        Ra::Core::VectorArray<Ra::Core::Triangle>* getTrianglesRw();

        const Ra::Core::Index* roIndexRead() const;

    private:
        Ra::Core::Index m_meshIndex;
        Ra::Core::Index m_aabbIndex;
        std::string m_contentName;
        bool m_deformable;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
