#ifndef FANCYMESHPLUGIN_FANCYMESHLOADINGDATA_HPP
#define FANCYMESHPLUGIN_FANCYMESHLOADINGDATA_HPP

#include <string>
#include <vector>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
    }
}

namespace FancyMeshPlugin
{
    struct RA_API FancyMeshData
    {
        Ra::Core::Vector4Array positions;
        Ra::Core::Vector4Array normals;
        Ra::Core::Vector4Array tangents;
        Ra::Core::Vector4Array bitangents;
        Ra::Core::Vector4Array texcoords;
        Ra::Core::Vector4Array colors;
        Ra::Core::Vector4Array weights;

        std::vector<uint>      indices;
    };

    struct RA_API FancyComponentData
    {
        RA_CORE_ALIGNED_NEW
        Ra::Core::Matrix4 transform;
        std::vector<FancyMeshData> meshes;

        std::string name;
        Ra::Engine::RenderTechnique* renderTechnique;
    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHLOADINGDATA_HPP
