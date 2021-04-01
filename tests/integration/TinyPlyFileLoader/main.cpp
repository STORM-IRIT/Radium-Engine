#include <Core/Asset/FileData.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Utils/Log.hpp>

#include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>

#include <memory>

int main( int /*argc*/, char** /*argv*/ ) {
    using namespace Ra::Core;
    using namespace Ra::Core::Asset;
    using namespace Ra::IO;

    TinyPlyFileLoader loader;
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/simple.ply" ) );
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/simple_normals.ply" ) );
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/buggy-1.ply" ) );
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/test_color_nonormals.ply" ) );
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/test_color_normals.ply" ) );
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/test_nocolor_normals.ply" ) );
    std::unique_ptr<FileData>( loader.loadFile( "../data/inputs/custom_attr.ply" ) );
    return 0;
}
