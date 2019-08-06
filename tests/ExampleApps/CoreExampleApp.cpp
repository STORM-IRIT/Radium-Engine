#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Utils/Log.hpp>

int main( int /*argc*/, char** /*argv*/ ) {
    using namespace Ra::Core;

    auto box  = Geometry::makeSharpBox( {1., 1., 1.} );
    auto topo = Geometry::TopologicalMesh( box );

    LOG( Utils::logINFO ) << topo.has_vertex_texcoords1D();
}
