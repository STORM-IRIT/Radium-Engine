#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <vector>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Vector.hpp>
#include <Engine/Renderer/Drawable.hpp>

namespace Ra { namespace Engine {

struct VertexData
{
    Core::Vector3 position;
//    Vector3 normal;
//    Vector3 tangent;
//    Vector3 texcoord;

    // TODO(Charly): Add skinning stuff
};

struct MeshData
{
    std::vector<VertexData> vertices;
    std::vector<uint>       indices;
};

class Mesh : public Drawable
{
public:
    Mesh(const std::string& name);
    virtual ~Mesh();

    void loadGeometry(const MeshData& data, bool computeNormals = false);
    const MeshData& getMeshData() const { return m_data; }

    virtual void draw() override;

private:
    Mesh(const Mesh&) = delete;
    void operator=(const Mesh&) = delete;
    void initGL();
    void computeNormals();

private:
    bool m_initialized;

    uint m_numVertices;
    uint m_numIndices;

    uint m_vao;
    uint m_vbo;
    uint m_ibo;

    MeshData m_data;

    // TODO(Charly): BBox ?
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESH_HPP
