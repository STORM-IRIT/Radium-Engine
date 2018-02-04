#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <FancyMeshPluginMacros.hpp>

#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <Core/File/GeometryData.hpp>

#include <Engine/Component/Component.hpp>

namespace Ra
{
    namespace Engine
    {
        class RenderTechnique;
        class Mesh;
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
        using DuplicateTable = Ra::Asset::GeometryData::DuplicateTable;

        FancyMeshComponent( const std::string& name, bool deformable = true );
        virtual ~FancyMeshComponent();


        void initialize() override;

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
        const DuplicateTable* getDuplicateTableOutput() const;
        Ra::Core::TriangleMesh* getMeshRw();
        void setMeshInput( const Ra::Core::TriangleMesh* mesh );
        Ra::Core::Vector3Array* getVerticesRw();
        Ra::Core::Vector3Array* getNormalsRw();
        Ra::Core::VectorArray<Ra::Core::Triangle>* getTrianglesRw();

        const Ra::Core::Index* roIndexRead() const;

    private:
        // The duplicate table for vertices, according to vertices position and normals in the mesh data.
        // Let M be a mesh with a total of n vertices, of which m are duplicates of others (same position but different attributes).
        // Then, the duplicate table for M has n entries such that m_duplicateTable[i] is the index
        // for the duplicated attributes of vertice i in M data (position, one-ring normal, ...) while i is the index
        // for its non duplicated attributes (texture coordinates, face normal, tangent vector, ...).
        // Note: if duplicates have NOT been loaded, then m_duplicateTable[i] == i.
        DuplicateTable m_duplicateTable;

        Ra::Core::Index m_meshIndex;
        Ra::Core::Index m_aabbIndex;
        std::string m_contentName;
        bool m_deformable;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
