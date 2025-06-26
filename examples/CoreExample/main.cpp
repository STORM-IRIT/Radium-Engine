#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Geometry/IndexedGeometry.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Utils/Log.hpp>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <unordered_map>

int main( int /*argc*/, char** /*argv*/ ) {
    using namespace Ra::Core;

    auto box  = Geometry::makeSharpBox( { 1., 1., 1. } );
    auto topo = Geometry::TopologicalMesh( box );

    auto resourcesPaths { Resources::getBasePath() };
    LOG( Utils::logINFO ) << "Executable is located at : " << *resourcesPaths;
    LOG( Utils::logINFO ) << topo.has_vertex_texcoords1D();

    return 0;
}
