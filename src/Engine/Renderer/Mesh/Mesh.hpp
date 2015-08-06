#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <vector>
#include <array>
#include <map>

#include <Core/CoreMacros.hpp>
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

        class RA_API Mesh
        {
        public:
            enum DataType
            {
                VERTEX_POSITION = 0,
                VERTEX_NORMAL,
                VERTEX_TANGENT,
                VERTEX_BITANGENT,
                VERTEX_TEXCOORD,

                MAX_DATATYPES
            };

        private:
            typedef Core::VectorArray<Core::Vector3> Vector3Array;
        public:
            Mesh ( const std::string& name );
            ~Mesh();

            const std::string& getName() const
            {
                return m_name;
            }

            /// GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_ADJACENCY, etc...
            void setRenderMode ( const GLenum& mode );

            void loadGeometry ( const Vector3Array& positions, const std::vector<uint>& indices );
            void addData ( const DataType& type, const Vector3Array& position );

            const Vector3Array& getData ( const DataType& type ) const
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

        private:
            Mesh ( const Mesh& ) = delete;
            void operator= ( const Mesh& ) = delete;

        private:
            std::string m_name;
            bool m_isDirty;

            uint m_vao;
            GLenum m_renderMode;

            std::array<Vector3Array, MAX_DATATYPES> m_data;
            std::array<GlBuffer<Core::Vector3>, MAX_DATATYPES> m_vbos;

            std::vector<uint> m_indices;
            GlBuffer<uint, GL_ELEMENT_ARRAY_BUFFER> m_ibo;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESH_HPP
