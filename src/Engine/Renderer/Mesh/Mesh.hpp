#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <array>
#include <map>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{
    namespace Engine
    {

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
        /// Internally all data is stored as arrays of Vector4 but convenience
        /// functions allow to add input data as Vector3.
        class RA_ENGINE_API Mesh
        {
        public:
            /// List of all possible vertex attributes.

            /// Information which is in the mesh
            enum MeshData : uint
            {
                INDEX = 0,
                VERTEX_POSITION,
                VERTEX_NORMAL,

                MAX_MESH
            };

            /// Extra vector 3 data
            enum Vec3Data : uint
            {
                VERTEX_TANGENT = 0,
                VERTEX_BITANGENT,
                VERTEX_TEXCOORD,

                MAX_VEC3
            };

            /// Extra vector 4 data
            enum Vec4Data : uint
            {
                VERTEX_COLOR = 0,
                VERTEX_WEIGHTS,

                MAX_VEC4
            };

            constexpr static uint MAX_DATA = MAX_MESH + MAX_VEC3 + MAX_VEC4;

        public:
            Mesh( const std::string& name, GLenum renderMode = GL_TRIANGLES );
            ~Mesh();

            const std::string& getName() const
            {
                return m_name;
            }

            /// GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_ADJACENCY, etc...
            void setRenderMode( GLenum mode );

            const Core::TriangleMesh& getGeometry() const { return m_mesh;}
            Core::TriangleMesh* getGeometryPtr()  { return &m_mesh;}

            void loadGeometry( const Core::TriangleMesh& mesh);
            void loadGeometry( const Core::Vector3Array& vertices, const std::vector<uint>& indices);

            void addData( const Vec3Data& type, const Core::Vector3Array& data);
            void addData( const Vec4Data& type, const Core::Vector4Array& data);

            const Core::Vector3Array& getData( const Vec3Data& type ) const
            {
                return m_v3Data[static_cast<uint>(type)];
            }

            const Core::Vector4Array& getData( const Vec4Data& type ) const
            {
                return m_v4Data[static_cast<uint>(type)];
            }

            void setDirty()
            {
                m_isDirty = true;
            }
            void updateGL();

            void render();

        private:
            template < typename VecArray >
            void sendGLData( const VecArray& arr, const uint vboIdx );


            Mesh( const Mesh& ) = delete;
            void operator= ( const Mesh& ) = delete;

        private:
            std::string m_name;

            uint m_vao;
            GLenum m_renderMode;

            Core::TriangleMesh m_mesh;

            std::array<Core::Vector3Array, MAX_VEC3 > m_v3Data;
            std::array<Core::Vector4Array, MAX_VEC4 > m_v4Data;

            // mesh, then Vec3 then vec 4 datas.
            std::array<uint, MAX_DATA> m_vbos = {{ 0 }};
            std::array<bool, MAX_DATA> m_dataDirty = {{ false }};
            bool m_isDirty;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESH_HPP
