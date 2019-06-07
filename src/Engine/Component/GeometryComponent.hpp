#ifndef RADIUMENGINE_GEOMETRY_COMPONENT_HPP
#define RADIUMENGINE_GEOMETRY_COMPONENT_HPP

#include <Core/Asset/GeometryData.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Engine/Component/Component.hpp>

namespace Ra {
namespace Engine {
class Mesh;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
/*!
 * \brief Main class to convert Ra::Core::Asset::GeometryData to Ra::Engine::Mesh
 *
 * Exports access to the mesh geometry:
 *  - TriangleMesh: get, rw (set vertices, normals and triangles dirty)
 *  - Vertices: rw (if deformable)
 *  - normals: rw (if deformable)
 *  - triangles: rw (if deformable)
 */
class RA_ENGINE_API TriangleMeshComponent : public Component
{
  public:
    TriangleMeshComponent( const std::string& name,
                           Entity* entity,
                           const Ra::Core::Asset::GeometryData* data );

    /*!
     * Constructor from an existing mesh
     * \warning Moves the mesh and takes its ownership
     */
    TriangleMeshComponent( const std::string& name,
                           Entity* entity,
                           Core::Geometry::TriangleMesh&& mesh,
                           Core::Asset::MaterialData* mat = nullptr );

    ~TriangleMeshComponent() override;

    void initialize() override;

    /// Returns the current display geometry.
    const Ra::Core::Geometry::TriangleMesh& getMesh() const;
    Mesh* getDisplayMesh();

  public:
    // Component communication management
    void setupIO( const std::string& id );
    void setContentName( const std::string& name );
    void setDeformable( bool b );

    /// Returns the index of the associated RO (the display mesh)
    Ra::Core::Utils::Index getRenderObjectIndex() const;

  private:
    void generateTriangleMesh( const Ra::Core::Asset::GeometryData* data );

    void finalizeROFromGeometry( const Core::Asset::MaterialData* data );

    // Give access to the mesh and (if deformable) to update it
    const Ra::Core::Geometry::TriangleMesh* getMeshOutput() const;
    Ra::Core::Geometry::TriangleMesh* getMeshRw();
    Ra::Core::Vector3Array* getVerticesRw();
    Ra::Core::Vector3Array* getNormalsRw();
    Ra::Core::VectorArray<Ra::Core::Vector3ui>* getTrianglesRw();

    const Ra::Core::Utils::Index* roIndexRead() const;

  private:
    Ra::Core::Utils::Index m_meshIndex{};
    std::string m_contentName{};
    // directly hold a reference to the displayMesh to simplify accesses in handlers
    std::shared_ptr<Mesh> m_displayMesh{nullptr};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_COMPONENT_HPP
