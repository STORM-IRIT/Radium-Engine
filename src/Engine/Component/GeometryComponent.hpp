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
/**
 * \brief Main class to convert Ra::Core::Asset::GeometryData to Ra::Engine::Mesh
 *
 * Exports access to the mesh geometry.
 *
 * One can get read-only access (Getter callback) to:
 *  - the TriangleMesh,
 *  - the RenderObject Index.
 *
 * One can get write-only access (Setter callback) to:
 *  - the TriangleMesh.
 *
 * One can get read-write access (RW callback) to:
 *  - the TriangleMesh,
 *  - the TriangleMesh vertices (data id post-fixed with "_v"),
 *  - the TriangleMesh normals (data id post-fixed with "_n"),
 *  - the TriangleMesh triangles (data id post-fixed with "_t").
 *
 * \note Calling the RW callbacks will enforce an update of the Engine::Mesh.
 */
class RA_ENGINE_API GeometryComponent : public Component {
  public:
    GeometryComponent( const std::string& name, bool deformable, Entity* entity );
    ~GeometryComponent() override;

    void initialize() override;

    /**
     * Creates a RenderObject for the given TriangleMesh \p mesh, with the given name.
     */
    void addMeshRenderObject( const Ra::Core::Geometry::TriangleMesh& mesh,
                              const std::string& name );

    /**
     * Creates a RenderObject from the data contained in \p data.
     */
    void handleMeshLoading( const Ra::Core::Asset::GeometryData* data );

    /**
     * Returns the index of the associated RO (the display mesh)
     */
    Ra::Core::Utils::Index getRenderObjectIndex() const;

    /**
     * Returns the current display geometry.
     */
    const Ra::Core::Geometry::TriangleMesh& getMesh() const;

  public:
    /**
     * Component communication management.
     */
    void setupIO( const std::string& id );

    /**
     * Set the data id for Component Communication.
     */
    void setContentName( const std::string& name );

    /**
     * Set whether the mesh is deformable.
     */
    void setDeformable( bool b );

  private:
    /**
     * Return the Engine::Mesh.
     */
    const Mesh& getDisplayMesh() const;

    /**
     * Return the Engine::Mesh.
     */
    Mesh& getDisplayMesh();

    /** \name CC callback functions.
     * Give access to the mesh and enables to update it (if deformable).
     */
    /// \{

    /**
     * Getter for the TriangleMesh.
     */
    const Ra::Core::Geometry::TriangleMesh* getMeshOutput() const;

    /**
     * Read/write access for the TriangleMesh.
     */
    Ra::Core::Geometry::TriangleMesh* getMeshRw();

    /**
     * Setter for the TriangleMesh.
     */
    void setMeshInput( const Core::Geometry::TriangleMesh* mesh );

    /**
     * Read/write access to the TriangleMesh vertices.
     */
    Ra::Core::Vector3Array* getVerticesRw();

    /**
     * Read/write access to the TriangleMesh normals.
     */
    Ra::Core::Vector3Array* getNormalsRw();

    /**
     * Read/write access to the TriangleMesh triangles.
     */
    Ra::Core::VectorArray<Ra::Core::Vector3ui>* getTrianglesRw();

    /**
     * Getter for the RenderObject Index.
     */
    const Ra::Core::Utils::Index* roIndexRead() const;
    /// \}

  private:
    Ra::Core::Utils::Index m_meshIndex{}; ///< The RenderObject Index.
    Ra::Core::Utils::Index m_aabbIndex{}; ///< [deprecated] Unused.
    std::string m_contentName{};          ///< The data id for CC.
    bool m_deformable{false};             ///< Whether the mesh is deformable.
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_COMPONENT_HPP
