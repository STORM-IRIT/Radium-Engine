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
        VERTEX_POSITION, ///< Vertex positions
        VERTEX_NORMAL,   ///< Vertex normals

        MAX_MESH
    };

    /// Optional vector 3 data.
    enum Vec3Data : uint {
        VERTEX_TANGENT = 0, ///< Vertex tangent 1
        VERTEX_BITANGENT,   ///< Vertex tangent 2
        VERTEX_TEXCOORD,    ///< U,V  texture coords (last coordinate not used)

        MAX_VEC3
    };

    /// Optional vector 4 data
    enum Vec4Data : uint {
        VERTEX_COLOR = 0,  ///< RGBA color.
        VERTEX_WEIGHTS,    ///< Skinning weights (not used)
        VERTEX_WEIGHT_IDX, ///< Associated weight bones

        MAX_VEC4
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

    /// Total number of vertex attributes.
    constexpr static uint MAX_DATA = MAX_MESH + MAX_VEC3 + MAX_VEC4;

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

    /// Access the additionnal data arrays by type.

    /// Mark one of the data types as dirty, forcing an update of the openGL buffer.
    void setDirty( const MeshData& type );

    /// Mark one of the data types as dirty, forcing an update of the openGL buffer.
    void setDirty( const Vec3Data& type );

    /// Mark one of the data types as dirty, forcing an update of the openGL buffer.
    void setDirty( const Vec4Data& type );

    /**
     * This function is called at the start of the rendering. It will update the
     * necessary openGL buffers.
     */
    void updateGL() override = 0;

    //@{
    /// Get the name expected for a given attrib.
    static inline std::string getAttribName( MeshData type );
    static inline std::string getAttribName( Vec3Data type );
    static inline std::string getAttribName( Vec4Data type );
    //@}

  protected:
    /// Update the picking render mode according to the object render mode
    void updatePickingRenderMode();

    class AttribObserver
    {
      public:
        explicit AttribObserver( VaoDisplayable* vao, int idx ) : m_vao( vao ), m_idx( idx ) {}
        void operator()() {

            auto result = std::find_if( m_vao->m_handleToBuffer.begin(),
                                        m_vao->m_handleToBuffer.end(),
                                        [this]( const auto& mo ) { return mo.second == this->m_idx; } );

            LOG( logDEBUG ) << "set : " << m_idx << " " << result->first << " dirty\n";
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

    // Combined arrays store the flags in this order Mesh, then Vec3 then Vec4 data.
    // Following the enum declaration above.
    // Our first VBO index is actually the indices buffer index.
    // The following are for vertex data.
    // Each data type has a corresponding openGL attribute number, which is
    // vbo index - 1 (thus vertex position is VBO number 1 but attribute 0).
    std::unique_ptr<globjects::Buffer> m_indices;
    bool m_indicesDirty{true};
    std::vector<std::unique_ptr<globjects::Buffer>> m_vbos;
    std::vector<bool> m_dataDirty;
    std::map<std::string, int> m_handleToBuffer;

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

    /// Returns the underlying AbstractGeometry, which is in fact a TriangleMesh
    /// \see getTriangleMesh
    inline const Core::Geometry::AbstractGeometry& getAbstractGeometry() const override;
    inline Core::Geometry::AbstractGeometry& getAbstractGeometry() override;

    /// Returns the underlying TriangleMesh
    inline const CoreGeometry& getTriangleMesh() const;
    inline CoreGeometry& getTriangleMesh();

    inline size_t getNumVertices() const override { return m_mesh.vertices().size(); }

    /// Use the given geometry as base for a display mesh. Normals are optionnal.
    void loadGeometry( CoreGeometry&& mesh ) { CORE_ASSERT( false, "must be specialized" ); }

    /**
     * Set additionnal vertex data.
     * Initialize vertexAttrib if needed,
     * data must have the appropriate size (i.e. num vertex) or empty (to
     * remove the data)
     * Theses functions might disapear to use directly Core::Geometry::TriangleMesh attribs.
     *
     * \note Attributes names are computed by #getAttribName
     */
    template <typename Type, typename Vector>
    [[deprecated]] void addData( const Type& type, const Core::VectorArray<Vector>& data );
    template <typename Vector>
    void addData( const std::string& name, const Core::VectorArray<Vector>& data );

  protected:
    void addAttribObserver( const std::string& name ) {
        CORE_ASSERT( false, "must be specialized" );
    }
    /// Base geometry : vertices, triangles and normals
    // must be the last one, to be delete first (and notify in class observer's)
    CoreGeometry m_mesh;
};

template <>
void DisplayableGeometry<Core::Geometry::TriangleMesh>::addAttribObserver(
    const std::string& name );
template <>
void DisplayableGeometry<Core::Geometry::TriangleMesh>::loadGeometry(
    Core::Geometry::TriangleMesh&& );

class Mesh : public DisplayableGeometry<Core::Geometry::TriangleMesh>
{
    using base = DisplayableGeometry<Core::Geometry::TriangleMesh>;

  public:
    using DisplayableGeometry<Core::Geometry::TriangleMesh>::DisplayableGeometry;
    size_t getNumFaces() const override;
    /// Draw the mesh.
    void render( const ShaderProgram* prog ) override;

    void updateGL() override;

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

  private:
    void autoVertexAttribPointer( const ShaderProgram* prog );
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Mesh/Mesh.inl>

#endif // RADIUMENGINE_MESH_HPP
