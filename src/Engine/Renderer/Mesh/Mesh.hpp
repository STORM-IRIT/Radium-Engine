#ifndef RADIUMENGINE_MESH_HPP
#define RADIUMENGINE_MESH_HPP

#include <vector>
#include <map>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/OpenGL/GLBuffer.hpp>

namespace Ra { namespace Engine {

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
		VERTEX_TEXCOORD
	};

private:
    typedef Core::VectorArray<Core::Vector3> Vector3Array;
public:
    Mesh(const std::string& name);
    ~Mesh();

    const std::string& getName() const { return m_name; }

	/// GL_POINTS, GL_LINES, GL_TRIANGLES, GL_TRIANGLE_ADJACENCY, etc...
	void setRenderMode(const GLenum& mode);

	void loadGeometry(const Vector3Array& positions, const std::vector<uint>& indices);
	void addData(const DataType& type, const Vector3Array& position);

	const Vector3Array& getData(const DataType& type) const { return m_data.find(type)->second; }
	const std::vector<uint>& getIndices() const { return m_indices; }

    void setDirty() { m_isDirty = true; }
    void updateGL();

    void render();

private:
    Mesh(const Mesh&) = delete;
    void operator=(const Mesh&) = delete;

private:
    std::string m_name;
    bool m_isDirty;

	std::map<DataType, Vector3Array> m_data;
	std::map<DataType, GlBuffer<Core::Vector3>> m_vbos;
	
	std::vector<uint> m_indices;
	GlBuffer<uint> m_ibo;

    uint m_vao;
	GLenum m_renderMode;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESH_HPP
