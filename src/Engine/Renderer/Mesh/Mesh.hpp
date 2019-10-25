#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Displayable/DisplayableObject.hpp>

#include <array>
#include <map>
#include <vector>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Utils/Color.hpp>

#include <Core/Utils/Log.hpp>

namespace globjects {

class VertexArray;
class Buffer;

} // namespace globjects

namespace Ra {
namespace Engine {
class ShaderProgram;
using namespace Ra::Core::Utils;

/// VAO + VBO attributes management,
/// also manage draw calls
class Vao
{
    /// \todo not used for now ... but may be if we allow multiple vao per mesh
    std::unique_ptr<globjects::VertexArray> m_vao;
    std::vector<std::unique_ptr<globjects::Buffer>> m_vbos;
    std::vector<bool> m_dataDirty;
    std::map<std::string, int> m_handleToBuffer;
};

/**
 * A class representing an openGL general mesh to be displayed.
 * It stores the vertex attributes, indices, and can be rendered
 * with a specific render mode (e.g. GL_TRIANGLES or GL_LINES).
 * It maintains the attributes and keeps them in sync with the GPU.
 * \note Attribute names are used to automatic location binding when using shaders.
 */
class RA_ENGINE_API VaoDisplayable : public Displayable
{
  public:
    /// \name List of all possible vertex attributes.
    ///@{
    // This is also the layout of the "dirty bit" and "vbo" arrays.

    /// Information which is in the mesh geometry
    enum MeshData : uint {
        VERTEX_POSITION,   ///< Vertex positions
        VERTEX_NORMAL,     ///< Vertex normals
        VERTEX_TANGENT,    ///< Vertex tangent 1
        VERTEX_BITANGENT,  ///< Vertex tangent 2
        VERTEX_TEXCOORD,   ///< U,V  texture coords (last coordinate not used)
        VERTEX_COLOR,      ///< RGBA color.
        VERTEX_WEIGHTS,    ///< Skinning weights (not used)
        VERTEX_WEIGHT_IDX, ///< Associated weight bones

        MAX_DATA
    };
    ///@}

    /**
     * Mesh render mode enum.
     * values taken from OpenGL specification
     */
    enum MeshRenderMode : uint {
        RM_POINTS                   = 0x0000,
        RM_LINES                    = 0x0001, // decimal value: 1
        RM_LINE_LOOP                = 0x0002, // decimal value: 2
        RM_LINE_STRIP               = 0x0003, // decimal value: 3
        RM_TRIANGLES                = 0x0004, // decimal value: 4
        RM_TRIANGLE_STRIP           = 0x0005, // decimal value: 5
        RM_TRIANGLE_FAN             = 0x0006, // decimal value: 6
        RM_QUADS                    = 0x0007, // decimal value: 7
        RM_QUAD_STRIP               = 0x0008, // decimal value: 8
        RM_POLYGON                  = 0x0009, // decimal value: 9
        RM_LINES_ADJACENCY          = 0x000A, // decimal value: 10
        RM_LINE_STRIP_ADJACENCY     = 0x000B, // decimal value: 11
        RM_TRIANGLES_ADJACENCY      = 0x000C, // decimal value: 12
        RM_TRIANGLE_STRIP_ADJACENCY = 0x000D, // decimal value: 13
        RM_PATCHES                  = 0x000E, // decimal value: 14
    };

  public:
    explicit VaoDisplayable( const std::string& name, MeshRenderMode renderMode = RM_TRIANGLES );
    VaoDisplayable( const VaoDisplayable& rhs ) = delete;
    void operator=( const VaoDisplayable& rhs ) = delete;

    // no need to detach listener since TriangleMesh is owned by Mesh.
    ~VaoDisplayable(){};

    using Displayable::getName;

    /// GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_ADJACENCY, etc...
    inline void setRenderMode( MeshRenderMode mode );
    MeshRenderMode getRenderMode() const { return m_renderMode; }

    /// \name Mark one of the data types as dirty, forcing an update of the openGL buffer.
    ///@{

    /// \param type the data to set to dirty
    void setDirty( const MeshData& type );

    /// \param name, the data buffer name to set to dirty
    void setDirty( const std::string& name );

    /// \param index, the data buffer index to set to dirty. If index is greater
    /// than then number of buffer, this function as no effect
    void setDirty( unsigned int index );

    ///@}

    /**
     * This function is called at the start of the rendering. It will update the
     * necessary openGL buffers.
     */
    void updateGL() override = 0;

    //@{
    /// Get the name expected for a given attrib.
    static inline std::string getAttribName( MeshData type );
    //@}

  protected:
    /// Update the picking render mode according to the object render mode
    void updatePickingRenderMode();

    class AttribObserver
    {
      public:
        explicit AttribObserver( VaoDisplayable* vao, int idx ) : m_vao( vao ), m_idx( idx ) {}
        void operator()() {
            m_vao->m_dataDirty[m_idx] = true;
            m_vao->m_isDirty          = true;
        }

      private:
        VaoDisplayable* m_vao;
        int m_idx;
    };

  protected:
    std::unique_ptr<globjects::VertexArray> m_vao;

    MeshRenderMode m_renderMode{
        MeshRenderMode::RM_TRIANGLES}; /// Render mode (GL_TRIANGLES or GL_LINES, etc.)

    std::vector<std::unique_ptr<globjects::Buffer>> m_vbos;
    std::vector<bool> m_dataDirty;

    // Geometry attrib name (std::string) to buffer id (int)
    // buffer id are indices in m_vbos
    std::map<std::string, unsigned int> m_handleToBuffer;

    /// number of elements to draw. For triangles this is 3*numTriangles
    /// but not for lines.
    size_t m_numElements{0};

    /// General dirty bit of the mesh. Must be equivalent of the  "or" of the other dirty flags.
    /// an empty mesh is not dirty
    bool m_isDirty{false};
};

template <typename T>
class DisplayableGeometry : public VaoDisplayable
{
    using CoreGeometry = T;

  public:
    using VaoDisplayable::VaoDisplayable;
    explicit DisplayableGeometry( const std::string& name,
                                  CoreGeometry&& geom,
                                  MeshRenderMode renderMode = RM_TRIANGLES ) :
        VaoDisplayable( name, renderMode ) {
        loadGeometry( std::move( geom ) );
    }

    /// Returns the underlying AbstractGeometry, which is in fact a TriangleMesh
    /// \see getTriangleMesh
    inline const Core::Geometry::AbstractGeometry& getAbstractGeometry() const override;
    inline Core::Geometry::AbstractGeometry& getAbstractGeometry() override;

    /// Returns the underlying TriangleMesh
    inline const CoreGeometry& getTriangleMesh() const;
    inline CoreGeometry& getTriangleMesh();

    /// Helper function that calls Ra::Core::CoreGeometry::addAttrib()
    template <typename A>
    inline Ra::Core::Utils::AttribHandle<A> addAttrib( const std::string& name,
                                                       const typename Core::VectorArray<A>& data ) {
        return m_mesh.addAttrib( name, data );
    }

    inline size_t getNumVertices() const override { return m_mesh.vertices().size(); }

    /// Use the given geometry as base for a display mesh. Normals are optionnal.
    void loadGeometry( CoreGeometry&& mesh ) { CORE_ASSERT( false, "must be specialized" ); }
    void updateGL() override;

  protected:
    virtual void updateGL_specific_impl(){};
    void loadGeometry_common( CoreGeometry&& mesh );
    void autoVertexAttribPointer( const ShaderProgram* prog );
    void addAttribObserver( const std::string& name );

    CoreGeometry m_mesh;
};

template <>
void DisplayableGeometry<Core::Geometry::TriangleMesh>::loadGeometry(
    Core::Geometry::TriangleMesh&& );

template <>
void DisplayableGeometry<Core::Geometry::LineMesh>::loadGeometry( Core::Geometry::LineMesh&& );

template <>
void DisplayableGeometry<Core::Geometry::PointCloud>::loadGeometry( Core::Geometry::PointCloud&& );

class PointCloud : public DisplayableGeometry<Core::Geometry::PointCloud>
{
    using base = DisplayableGeometry<Core::Geometry::PointCloud>;

  public:
    using DisplayableGeometry<Core::Geometry::PointCloud>::DisplayableGeometry;
    void render( const ShaderProgram* prog ) override;

  protected:
    void updateGL_specific_impl() override;
};

class LineMesh : public DisplayableGeometry<Core::Geometry::LineMesh>
{
    using base = DisplayableGeometry<Core::Geometry::LineMesh>;

  public:
    using DisplayableGeometry<Core::Geometry::LineMesh>::DisplayableGeometry;
    void render( const ShaderProgram* prog ) override;

  protected:
    void updateGL_specific_impl() override;

  private:
    std::unique_ptr<globjects::Buffer> m_indices;
    bool m_indicesDirty{true};
};

class Mesh : public DisplayableGeometry<Core::Geometry::TriangleMesh>
{
    using base = DisplayableGeometry<Core::Geometry::TriangleMesh>;

  public:
    using DisplayableGeometry<Core::Geometry::TriangleMesh>::DisplayableGeometry;
    size_t getNumFaces() const override;
    /// Draw the mesh.
    void render( const ShaderProgram* prog ) override;

    /**
     * Use the given vertices and indices to build a display mesh according to
     * the MeshRenderMode.
     * \note This has to be used for non RM_TRIANGLES meshes only.
     * \note Also removes all vertex attributes.
     * \warning This might disappear when line meshes will be managed.
     */
    // Had to keep this for line meshes and Render Primitives.
    using base::loadGeometry;
    [[deprecated]] void loadGeometry( const Core::Vector3Array& vertices,
                                      const std::vector<uint>& indices );

  protected:
    void updateGL_specific_impl() override;

  private:
    std::unique_ptr<globjects::Buffer> m_indices;
    bool m_indicesDirty{true};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Mesh/Mesh.inl>

#endif // RADIUMENGINE_MESH_HPP
