#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <Engine/RaEngine.hpp>

#include <array>
#include <map>
#include <vector>

#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Engine {

// FIXME(Charly): If I want to draw a mesh as lines, points, etc,
//                should I send lines, ... to the GPU, or handle the way
//                I want them displayed in a geometry shader, and always
//                send adjacent triangles to the GPU ?
//                The latter solution would be faster (no if / else) in
//                the updateGL, draw methods, but would require more work
//                for the plugin developper (or we can just provide shaders
//                for this kind of renderings ...)

/// A class representing an openGL general mesh to be displayed.
/// It stores the vertex attributes, indices, and can be rendered
/// with a specific render mode (e.g. GL_TRIANGLES or GL_LINES).
/// It maintains the attributes and keeps them in sync with the GPU.
class RA_ENGINE_API Mesh {
  public:
    /// List of all possible vertex attributes.

    // This is also the layout of the "dirty bit" and "vbo" arrays.

    /// Information which is in the mesh geometry
    enum MeshData : uint {
        INDEX = 0,       /// Vertex indices
        VERTEX_POSITION, /// Vertex positions
        VERTEX_NORMAL,   /// Vertex normals

        MAX_MESH
    };

    /// Optional vector 3 data.
    enum Vec3Data : uint {
        VERTEX_TANGENT = 0, /// Vertex tangent 1
        VERTEX_BITANGENT,   /// Vertex tangent 2
        VERTEX_TEXCOORD,    /// U,V  texture coords (last coordinate not used)

        MAX_VEC3
    };

    /// Optional vector 4 data
    enum Vec4Data : uint {
        VERTEX_COLOR = 0,  /// RGBA color.
        VERTEX_WEIGHTS,    /// Skinning weights (not used)
        VERTEX_WEIGHT_IDX, /// Associated weight bones

        MAX_VEC4
    };

    /** Mesh render mode enum.
     * values taken from OpenGL specification
     */
    enum MeshRenderMode : uint {
        RM_POINTS = 0x0000,
        RM_LINES = 0x0001,                    // decimal value: 1
        RM_LINE_LOOP = 0x0002,                // decimal value: 2
        RM_LINE_STRIP = 0x0003,               // decimal value: 3
        RM_TRIANGLES = 0x0004,                // decimal value: 4
        RM_TRIANGLE_STRIP = 0x0005,           // decimal value: 5
        RM_TRIANGLE_FAN = 0x0006,             // decimal value: 6
        RM_QUADS = 0x0007,                    // decimal value: 7
        RM_QUAD_STRIP = 0x0008,               // decimal value: 8
        RM_POLYGON = 0x0009,                  // decimal value: 9
        RM_LINES_ADJACENCY = 0x000A,          // decimal value: 10
        RM_LINE_STRIP_ADJACENCY = 0x000B,     // decimal value: 11
        RM_TRIANGLES_ADJACENCY = 0x000C,      // decimal value: 12
        RM_TRIANGLE_STRIP_ADJACENCY = 0x000D, // decimal value: 13
        RM_PATCHES = 0x000E,                  // decimal value: 14
    };

    /// Total number of vertex attributes.
    constexpr static uint MAX_DATA = MAX_MESH + MAX_VEC3 + MAX_VEC4;

  public:
    Mesh( const std::string& name, MeshRenderMode renderMode = RM_TRIANGLES );
    ~Mesh();

    /// Returns the name of the mesh.
    inline const std::string& getName() const;

    /// GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_ADJACENCY, etc...
    inline void setRenderMode( MeshRenderMode mode );
    MeshRenderMode getRenderMode() const { return m_renderMode; }

    /// Returns the underlying triangle mesh.
    inline const Core::TriangleMesh& getGeometry() const;
    inline Core::TriangleMesh& getGeometry();

    /// Use the given geometry as base for a display mesh. Normals are optionnal.
    void loadGeometry( const Core::TriangleMesh& mesh );

    void updateMeshGeometry( MeshData type, const Core::Vector3Array& data );

    // TODO (val) : remove this function (it is used mostly in the display primitives)
    void loadGeometry( const Core::Vector3Array& vertices, const std::vector<uint>& indices );

    /// Set additionnal vertex data.
    /// Initialize vertexAttrib if needed,
    /// data must have the appropriate size (i.e. num vertex) or empty (to
    /// remove the data)
    void addData( const Vec3Data& type, const Core::Vector3Array& data );
    /// Load additionnal vertex data.
    /// \return False for invalid or empty attrib
    void addData( const Vec4Data& type, const Core::Vector4Array& data );

    /// Access the additionnal data arrays by type.
    inline const Core::Vector3Array& getData( const Vec3Data& type ) const;
    inline const Core::Vector4Array& getData( const Vec4Data& type ) const;
    inline Core::Vector3Array& getData( const Vec3Data& type );
    inline Core::Vector4Array& getData( const Vec4Data& type );

    /// Mark one of the data types as dirty, forcing an update of the openGL buffer.
    inline void setDirty( const MeshData& type );
    inline void setDirty( const Vec3Data& type );
    inline void setDirty( const Vec4Data& type );

    /// This function is called at the start of the rendering. It will update the
    /// necessary openGL buffers.
    void updateGL();

    /// Draw the mesh.
    void render();

  private:
    Mesh( const Mesh& rhs ) = delete;
    void operator=( const Mesh& rhs ) = delete;

    /// Helper function to send buffer data to openGL.
    template <typename type>
    friend void sendGLData( Ra::Engine::Mesh* mesh, const Ra::Core::VectorArray<type>& arr,
                            const uint vboIdx );

  private:
    std::string m_name; /// Name of the mesh.

    uint m_vao;                  /// Index of our openGL VAO
    MeshRenderMode m_renderMode; /// Render mode (GL_TRIANGLES or GL_LINES, etc.)

    Core::TriangleMesh m_mesh; /// Base geometry : vertices, triangles
                               /// and normals

    ///\todo @dlyr cleanup this mechanism to have something
    /// extensible. Now the only attribs should be the one defined in
    /// the enums MeshData, Vec3Data, Vec4Data.

    /// Additionnal vertex vector 3 data handles, stored in Mesh, added
    std::array<Core::TriangleMesh::Vec3AttribHandle, MAX_VEC3> m_v3DataHandle;
    /// Additionnal vertex vector 4 data handles, stored in Mesh, added
    std::array<Core::TriangleMesh::Vec4AttribHandle, MAX_VEC4> m_v4DataHandle;
    Core::TriangleMesh::Vec4AttribHandle::Container m_dummy;

    // Combined arrays store the flags in this order Mesh, then Vec3 then Vec4 data.
    // Following the enum declaration above.
    // Our first VBO index is actually the indices buffer index.
    // The following are for vertex data.
    // Each data type has a corresponding openGL attribute number, which is
    // vbo index - 1 (thus vertex position is VBO number 1 but attribute 0).

    std::array<uint, MAX_DATA> m_vbos = {{0}};          /// Indices of our openGL VBOs.
    std::array<bool, MAX_DATA> m_dataDirty = {{false}}; /// Dirty bits of our vertex data.

    uint m_numElements; /// number of elements to draw. For triangles this is 3*numTriangles but not
                        /// for lines.
    // (val) : this is a bit hacky.

    bool m_isDirty; /// General dirty bit of the mesh.
    // TODO (Val) this flag could just be replaced by an efficient "or" of the other flags.
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Mesh/Mesh.inl>

#endif // RADIUMENGINE_MESH_HPP
