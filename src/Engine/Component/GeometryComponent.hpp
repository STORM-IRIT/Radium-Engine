#ifndef RADIUMENGINE_GEOMETRY_COMPONENT_HPP
#    define RADIUMENGINE_GEOMETRY_COMPONENT_

#    include <Core/File/GeometryData.hpp>
#    include <Core/Mesh/MeshTypes.hpp>
#    include <Core/Mesh/TriangleMesh.hpp>

#    include <Engine/Component/Component.hpp>

namespace Ra {
namespace Engine {
class RenderTechnique;
class Mesh;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
/*!
 * \brief Main class to convert Ra::Asset::GeometryData to Ra::Engine::Mesh
 *
 * Exports access to the mesh geometry:
 *  - TriangleMesh: get, rw (set vertices, normals and triangles dirty)
 *  - Vertices: rw (if deformable)
 *  - normals: rw (if deformable)
 *  - triangles: rw (if deformable)
 */
class RA_ENGINE_API GeometryComponent : public Component {
  public:
    GeometryComponent( const std::string& name, bool deformable, Entity* entity );
    virtual ~GeometryComponent();

    void initialize() override;

    void addMeshRenderObject( const Ra::Core::TriangleMesh& mesh, const std::string& name );
    void handleMeshLoading( const Ra::Asset::GeometryData* data );

    /// Returns the index of the associated RO (the display mesh)
    Ra::Core::Index getRenderObjectIndex() const;

    /// Returns the current display geometry.
    const Ra::Core::TriangleMesh& getMesh() const;

  public:
    // Component communication management
    void setupIO( const std::string& id );
    void setContentName( const std::string name );
    void setDeformable( const bool b );

  private:
    const Mesh& getDisplayMesh() const;
    Mesh& getDisplayMesh();

    // Give access to the mesh and (if deformable) to update it
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

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_COMPONENT_
