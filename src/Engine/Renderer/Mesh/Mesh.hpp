#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <array>
#include <map>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/OpenGL/GlBuffer.hpp>

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
            enum DataType
            {
                VERTEX_POSITION = 0,
                VERTEX_NORMAL,
                VERTEX_TANGENT,
                VERTEX_BITANGENT,
                VERTEX_TEXCOORD,
                VERTEX_COLOR,
                VERTEX_WEIGHTS,

                MAX_DATATYPES
            };

        public:
            Mesh( const std::string& name, GLenum renderMode = GL_TRIANGLES );
            ~Mesh();

            const std::string& getName() const
            {
                return m_name;
            }

            /// GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_ADJACENCY, etc...
            void setRenderMode( GLenum mode );

            void loadGeometry( const Core::TriangleMesh& mesh);
            void loadGeometry( const Core::Vector3Array& positions, const std::vector<uint>& indices);
            void loadGeometry( const Core::Vector4Array& positions, const std::vector<uint>& indices );

            void addData( const DataType& type, const Core::Vector3Array& data);
            void addData( const DataType& type, const Core::Vector4Array& data);

            const Core::Vector4Array& getData( const DataType& type ) const
            {
                return m_data[type];
            }

            const std::vector<uint>& getIndices() const
            {
                return m_indices;
            }

            void setDirty()
            {
                m_isDirty = true;
            }
            void updateGL();

            void render();

            std::shared_ptr<Mesh> clone();

        private:
            Mesh( const Mesh& ) = delete;
            void operator= ( const Mesh& ) = delete;

        private:
            std::string m_name;
            bool m_isDirty;

            uint m_vao;
            GLenum m_renderMode;

            std::array<Core::Vector4Array, MAX_DATATYPES> m_data;
            std::array<uint, MAX_DATATYPES> m_vbos = {{ 0 }};
            std::array<bool, MAX_DATATYPES> m_dirtyArray = {{ false }};

            std::vector<uint> m_indices;
            uint m_ibo;
            bool m_iboDirty = false;
//            GlBuffer<uint, GL_ELEMENT_ARRAY_BUFFER> m_ibo;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESH_HPP
