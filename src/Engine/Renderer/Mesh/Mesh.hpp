#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <vector>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Vector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>

namespace Ra { namespace Engine {

// FIXME(Charly): If I want to draw a mesh as lines, points, etc,
//                should I send lines, ... to the GPU, or handle the way
//                I want them displayed in a geometry shader, and always
//                send adjacent triangles to the GPU ?
//                The latter solution would be faster (no if / else) in
//                the updateGL, draw methods, but would require more work
//                for the plugin developper (or we can just provide shaders
//                for this kind of renderings ...)

class Mesh
{
private:
    typedef Core::VectorArray<Core::Vector3> Vector3Array;
public:
    Mesh(const std::string& name);
    ~Mesh();

    const std::string& getName() const { return m_name; }

    void loadGeometry(const Core::TriangleMesh &data,
                      const Vector3Array& tangents = Vector3Array(),
                      const Vector3Array& bitangents = Vector3Array(),
                      const Vector3Array& texcoords = Vector3Array(),
                      bool computeNormals = false);
    const Core::TriangleMesh& getMeshData() const { return m_data; }

    void updateGL();

    void draw();

private:
    Mesh(const Mesh&) = delete;
    void operator=(const Mesh&) = delete;
    void computeNormals();
    void computeAdjacency();

private:
    std::string m_name;
    bool m_initialized;

    Core::TriangleMesh m_data;
    Vector3Array m_texcoords;
    Vector3Array m_tangents;
    Vector3Array m_bitangents;
    std::vector<uint> m_adjacentTriangles;

    uint m_vao;
    uint m_vbo;
    uint m_nbo;
    uint m_tbo;
    uint m_bbo;
    uint m_cbo;

    uint m_ibo;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESH_HPP
