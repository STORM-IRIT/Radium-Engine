#ifndef FANCYMESHPLUGIN_FANCYMESHLOADINGDATA_HPP
#define FANCYMESHPLUGIN_FANCYMESHLOADINGDATA_HPP

#include <string>
#include <vector>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra { namespace Engine { class RenderTechnique; } }

namespace Ra {
namespace Engine {

struct RA_API FancyMeshData
{
	Core::TriangleMesh mesh;
	Core::Vector3Array tangents;
	Core::Vector3Array bitangents;
	Core::Vector3Array texcoords;
};

struct RA_API FancyComponentData
{
    RA_CORE_ALIGNED_NEW
	Core::Matrix4 transform;
	std::vector<FancyMeshData> meshes;

	std::string name;
	RenderTechnique* renderTechnique;
};

} // namespace Engine
} // namespace Ra

#endif // FANCYMESHPLUGIN_FANCYMESHLOADINGDATA_HPP
