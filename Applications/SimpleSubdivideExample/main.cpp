#include <Core/File/deprecated/OBJFileManager.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>
#include <memory>
struct args
{
    bool valid;
    int iteration;
    std::string outputFilename;
    std::string inputFilename;
    std::unique_ptr<OpenMesh::Subdivider::Uniform::SubdividerT<Ra::Core::TopologicalMesh>>
        subdivider;
};

void printHelp( char* argv[] )
{
    std::cout << "Usage :\n"
              << argv[0] << " -i input.obj -o output -s type -n iteration  \n\n"
              << " .obj extension is added automatically to output filename\n"
              << "input\t\t the name (with .obj extension) of the file to load, if no input is "
                 "given, a simple cube is used\n"
              << "type \t\t is a string for the subdivider type name : catmull, loop\n"
              << "iteration \t (default is 1) is a positive integer to specify the number of "
                 "iteration of subdivision\n";
}

args processArgs( int argc, char* argv[] )
{
    args ret;
    bool outputFilenameSet{false};
    bool subdividerSet{false};
    ret.iteration = 1;

    for ( int i = 1; i < argc; i += 2 )
    {
        if ( i >= argc ) break;
        if ( std::string( argv[i] ) == std::string( "-i" ) )
        {
            if ( i + 1 < argc )
            {
                ret.inputFilename = argv[i + 1];
            }
        }
        else if ( std::string( argv[i] ) == std::string( "-o" ) )
        {
            if ( i + 1 < argc )
            {
                ret.outputFilename = argv[i + 1];
                outputFilenameSet  = true;
            }
        }
        else if ( std::string( argv[i] ) == std::string( "-s" ) )
        {
            if ( i + 1 < argc )
            {
                std::string a{argv[i + 1]};
                subdividerSet = true;
                if ( a == std::string( "catmull" ) )
                {
                    ret.subdivider = std::make_unique<
                        OpenMesh::Subdivider::Uniform::CatmullClarkT<Ra::Core::TopologicalMesh>>();
                }
                else if ( a == std::string( "loop" ) )
                {
                    ret.subdivider = std::make_unique<
                        OpenMesh::Subdivider::Uniform::LoopT<Ra::Core::TopologicalMesh>>();
                }
                else
                {
                    subdividerSet = false;
                }
            }
        }
        else if ( std::string( argv[i] ) == std::string( "-n" ) )
        {
            if ( i + 1 < argc )
            {
                ret.iteration = std::stoi( std::string( argv[i + 1] ) );
            }
        }
    }
    ret.valid = outputFilenameSet && subdividerSet;
    return ret;
}

int main( int argc, char* argv[] )
{
    args a = processArgs( argc, argv );
    if ( !a.valid )
    {
        printHelp( argv );
    }
    else
    {
        Ra::Core::TriangleMesh mesh;
        Ra::Core::OBJFileManager obj;
        if ( a.inputFilename.empty() )
        {
            mesh = Ra::Core::MeshUtils::makeBox();
        }
        else
        {
            obj.load( a.inputFilename, mesh );
        }

        LOG( logINFO ) << "in Mesh";
        for ( auto v : mesh.vertices() )
        {
            LOG( logINFO ) << v.transpose();
        }
        LOG( logINFO ) << "in Normals";
        for ( auto v : mesh.normals() )
        {
            LOG( logINFO ) << v.transpose();
        }

        float i           = 0;
        auto test_handle2 = mesh.attribManager().addAttrib<Ra::Core::Vector4>( "test vec4" );
        mesh.attribManager().getAttrib( test_handle2 ).resize( mesh.vertices().size() );
        for ( auto& v : mesh.attribManager().getAttrib( test_handle2 ).data() )
        {
            v = Ra::Core::Vector4( i, i, i, i );
            i += 1.f;
        }

        auto test_handle = mesh.attribManager().addAttrib<Ra::Core::Vector3>( "test vec3" );
        mesh.attribManager().getAttrib( test_handle ).resize( mesh.vertices().size() );

        for ( auto& v : mesh.attribManager().getAttrib( test_handle ).data() )
        {
            v = Ra::Core::Vector3( i, i, i );
            LOG( logINFO ) << v.transpose();
            i += 1.f;
        }

        Ra::Core::TopologicalMesh topologicalMesh( mesh );

        /*
                        a.subdivider->attach( topologicalMesh );
                        ( *a.subdivider )( a.iteration );
                        a.subdivider->detach();
                        topologicalMesh.triangulate();
        */
        mesh = topologicalMesh.toTriangleMesh();
        /*
                LOG( logINFO ) << "out Vec4";
                auto out_handle2 = mesh.attribManager().getAttribHandle<Ra::Core::Vector4>( "test
           vec4" ); for ( auto v : mesh.attribManager().getAttrib( out_handle2 ).data() )
                {
                    LOG( logINFO ) << v;
                }
        */

        LOG( logINFO ) << "out Mesh";
        for ( auto v : mesh.vertices() )
        {
            LOG( logINFO ) << v.transpose();
        }
        LOG( logINFO ) << "out Normals";
        for ( auto v : mesh.normals() )
        {
            LOG( logINFO ) << v.transpose();
        }

        LOG( logINFO ) << "out Vec3";

        auto out_handle = mesh.attribManager().getAttribHandle<Ra::Core::Vector3>( "test vec3" );
        for ( auto v : mesh.attribManager().getAttrib( out_handle ).data() )
        {
            LOG( logINFO ) << v.transpose();
        }

        auto out_handle2 = mesh.attribManager().getAttribHandle<Ra::Core::Vector4>( "test vec4" );
        for ( auto v : mesh.attribManager().getAttrib( out_handle2 ).data() )
        {
            LOG( logINFO ) << v.transpose();
        }

        // obj.save( a.outputFilename, mesh );
    }
    return 0;
}
