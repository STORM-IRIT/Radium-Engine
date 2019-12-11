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

// from .inl, temporary include, remove when compiles
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>

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
class RA_ENGINE_API Vao
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
class RA_ENGINE_API AttribArrayDisplayable : public Displayable
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
    explicit AttribArrayDisplayable( const std::string& name,
                                     MeshRenderMode renderMode = RM_TRIANGLES );
    AttribArrayDisplayable( const AttribArrayDisplayable& rhs ) = delete;
    void operator=( const AttribArrayDisplayable& rhs ) = delete;

    // no need to detach listener since TriangleMesh is owned by Mesh.
    ~AttribArrayDisplayable(){};

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
        explicit AttribObserver( AttribArrayDisplayable* displayable, int idx ) :
            m_displayable( displayable ),
            m_idx( idx ) {}
        void operator()() {
            m_displayable->m_dataDirty[m_idx] = true;
            m_displayable->m_isDirty          = true;
        }

      private:
        AttribArrayDisplayable* m_displayable;
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

    /// General dirty bit of the mesh. Must be equivalent of the  "or" of the other dirty flags.
    /// an empty mesh is not dirty
    bool m_isDirty{false};
};

class RA_ENGINE_API VaoIndices
{
  protected:
    std::unique_ptr<globjects::Buffer> m_indices;
    bool m_indicesDirty{true};
    /// number of elements to draw. For triangles this is 3*numTriangles
    /// but not for lines.
    size_t m_numElements{0};
};

/// This class handles an attrib array displayable on gpu only, without core
/// geometry. Use only when you don't need to access the cpu geometry again, or
/// when you need to specify special indices.
template <typename I>
class IndexedAttribArrayDisplayable : public AttribArrayDisplayable, public VaoIndices
{
    using IndexType          = I;
    using IndexContainerType = Ra::Core::AlignedStdVector<IndexType>;

    template <typename T>
    inline void addAttrib( const std::string& name,
                           const typename Ra::Core::Utils::Attrib<T>::Container& data ) {
        auto handle = m_attribManager.addAttrib<T>( name );
        m_attribManager.getAttrib( handle ).setData( data );
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
        m_isDirty = true;
    }

    template <typename T>
    inline void addAttrib( const std::string& name,
                           const typename Ra::Core ::Utils::Attrib<T>::Container&& data ) {
        auto handle = m_attribManager.addAttrib<T>( name );
        m_attribManager.getAttrib( handle ).setData( std::move( data ) );
        m_handleToBuffer[name] = m_dataDirty.size();
        m_dataDirty.push_back( true );
        m_vbos.emplace_back( nullptr );
        m_isDirty = true;
    }

    void updateGL() override {
        if ( m_isDirty )
        {
            // Check that our dirty bits are consistent.
            ON_ASSERT( bool dirtyTest = false;
                       for ( const auto& d
                             : m_dataDirty ) { dirtyTest = dirtyTest || d; } );
            CORE_ASSERT( dirtyTest == m_isDirty, "Dirty flags inconsistency" );

            if ( !m_indices )
            {
                m_indices      = globjects::Buffer::create();
                m_indicesDirty = true;
            }
            if ( m_indicesDirty )
            {
                m_indices->setData(
                    static_cast<gl::GLsizeiptr>( m_cpu_indices.size() * sizeof( IndexType ) ),
                    m_cpu_indices.data(),
                    GL_STATIC_DRAW );
                m_indicesDirty = false;
            }

            m_numElements = m_cpu_indices.size();

            if ( !m_vao ) { m_vao = globjects::VertexArray::create(); }
            m_vao->bind();
            m_vao->bindElementBuffer( m_indices.get() );
            m_vao->unbind();

            auto func = [this]( Ra::Core::Utils::AttribBase* b ) {
                auto idx = m_handleToBuffer[b->getName()];

                if ( m_dataDirty[idx] )
                {
                    if ( !m_vbos[idx] ) { m_vbos[idx] = globjects::Buffer::create(); }
                    m_vbos[idx]->setData( b->getBufferSize(), b->dataPtr(), GL_DYNAMIC_DRAW );
                    m_dataDirty[idx] = false;
                }
            };
            m_attribManager.for_each_attrib( func );
            GL_CHECK_ERROR;
            m_isDirty = false;
        }
    }

    void autoVertexAttribPointer( const ShaderProgram* prog ) {

        auto glprog           = prog->getProgramObject();
        gl::GLint attribCount = glprog->get( GL_ACTIVE_ATTRIBUTES );

        m_vao->bind();
        for ( GLint idx = 0; idx < attribCount; ++idx )
        {
            const gl::GLsizei bufSize = 256;
            gl::GLchar name[bufSize];
            gl::GLsizei length;
            gl::GLint size;
            gl::GLenum type;
            glprog->getActiveAttrib( idx, bufSize, &length, &size, &type, name );
            auto loc = glprog->getAttributeLocation( name );

            auto attribName = name; // m_translationTableShaderToMesh[name];
            auto attrib     = m_attribManager.getAttribBase( attribName );

            if ( attrib )
            {
                m_vao->enable( loc );
                auto binding = m_vao->binding( idx );
                binding->setAttribute( loc );
                CORE_ASSERT( m_vbos[m_handleToBuffer[attribName]].get(), "vbo is nullptr" );
                binding->setBuffer(
                    m_vbos[m_handleToBuffer[attribName]].get(), 0, attrib->getStride() );
                binding->setFormat( attrib->getElementSize(), GL_FLOAT );
            }
            else
            { m_vao->disable( loc ); }
        }

        m_vao->unbind();
    }

    void render( const ShaderProgram* prog ) override {
        if ( m_vao )
        {
            autoVertexAttribPointer( prog );
            m_vao->bind();
            m_vao->drawElements( static_cast<GLenum>( m_renderMode ),
                                 GLsizei( m_numElements ),
                                 GL_UNSIGNED_INT,
                                 nullptr );
            m_vao->unbind();
        }
    }

    IndexContainerType m_cpu_indices;
    AttribManager m_attribManager;
};

template <typename T>
class CoreGeometryDisplayable : public AttribArrayDisplayable
{
  public:
    using CoreGeometry = T;
    using AttribArrayDisplayable::AttribArrayDisplayable;
    explicit CoreGeometryDisplayable( const std::string& name,
                                      CoreGeometry&& geom,
                                      MeshRenderMode renderMode = RM_TRIANGLES ) :
        AttribArrayDisplayable( name, renderMode ) {
        loadGeometry( std::move( geom ) );
    }

    /// Returns the underlying AbstractGeometry, which is in fact a TriangleMesh
    /// \see getTriangleMesh
    inline const Core::Geometry::AbstractGeometry& getAbstractGeometry() const;
    inline Core::Geometry::AbstractGeometry& getAbstractGeometry();

    /// Returns the underlying TriangleMesh
    inline const CoreGeometry& getCoreGeometry() const;
    inline CoreGeometry& getCoreGeometry();

    /// Helper function that calls Ra::Core::CoreGeometry::addAttrib()
    template <typename A>
    inline Ra::Core::Utils::AttribHandle<A> addAttrib( const std::string& name,
                                                       const typename Core::VectorArray<A>& data ) {
        return m_mesh.addAttrib( name, data );
    }

    inline size_t getNumVertices() const override { return m_mesh.vertices().size(); }

    /// Use the given geometry as base for a display mesh. Normals are optionnal.
    virtual void loadGeometry( CoreGeometry&& mesh ) {
        CORE_ASSERT( false, "must be specialized" );
    }
    void updateGL() override;

    void setTranslation( const std::string& meshAttribName, const std::string& shaderAttribName );

  protected:
    virtual void updateGL_specific_impl(){};

    void loadGeometry_common( CoreGeometry&& mesh );
    void autoVertexAttribPointer( const ShaderProgram* prog );
    void addAttribObserver( const std::string& name );

    /// Core::Mesh attrib name to Render::Mesh attrib name
    using TranslationTable = std::map<std::string, std::string>;
    TranslationTable m_translationTableMeshToShader;
    TranslationTable m_translationTableShaderToMesh;

    CoreGeometry m_mesh;
};

class RA_ENGINE_API PointCloud : public CoreGeometryDisplayable<Core::Geometry::PointCloud>
{
    using base = CoreGeometryDisplayable<Core::Geometry::PointCloud>;

  public:
    using CoreGeometryDisplayable<Core::Geometry::PointCloud>::CoreGeometryDisplayable;
    void render( const ShaderProgram* prog ) override;

    void loadGeometry( Core::Geometry::PointCloud&& mesh ) override {
        loadGeometry_common( std::move( mesh ) );
    }

  protected:
    void updateGL_specific_impl() override;
};

template <typename T>
class IndexedGeometry : public CoreGeometryDisplayable<T>, public VaoIndices
{
  public:
    using base = CoreGeometryDisplayable<T>;
    using CoreGeometryDisplayable<T>::CoreGeometryDisplayable;
    explicit IndexedGeometry(
        const std::string& name,
        typename base::CoreGeometry&& geom,
        typename base::MeshRenderMode renderMode = base::MeshRenderMode::RM_TRIANGLES ) :
        base( name, renderMode ) {
        loadGeometry( std::move( geom ) );
    }

    void render( const ShaderProgram* prog ) override;

    void loadGeometry( T&& mesh ) override {
        m_numElements = mesh.m_indices.size() * base::CoreGeometry::IndexType::RowsAtCompileTime;
        base::loadGeometry_common( std::move( mesh ) );
    }

  protected:
    void updateGL_specific_impl();
};

class RA_ENGINE_API LineMesh : public IndexedGeometry<Core::Geometry::LineMesh>
{
    using base = IndexedGeometry<Core::Geometry::LineMesh>;

  public:
    using base::IndexedGeometry;
    explicit LineMesh( const std::string& name,
                       typename base::CoreGeometry&& geom,
                       typename base::MeshRenderMode renderMode = base::MeshRenderMode::RM_LINES ) :
        base( name, std::move( geom ), renderMode ) {}

  protected:
  private:
};

class RA_ENGINE_API Mesh : public IndexedGeometry<Core::Geometry::TriangleMesh>
{
    using base = IndexedGeometry<Core::Geometry::TriangleMesh>;

  public:
    using base::IndexedGeometry;
    size_t getNumFaces() const override;

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
  private:
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Mesh/Mesh.inl>

#endif // RADIUMENGINE_MESH_HPP
