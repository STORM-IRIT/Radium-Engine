#include <Core/File/deprecated/OBJFileManager.hpp>

namespace Ra {
namespace Core {

/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
OBJFileManager::OBJFileManager() : FileManager<TriangleMesh>() {}

/// ===============================================================================
/// DESTRUCTOR
/// ===============================================================================
OBJFileManager::~OBJFileManager() {}

/// ===============================================================================
/// INTERFACE
/// ===============================================================================
std::string OBJFileManager::fileExtension() const {
    return "obj";
}

bool OBJFileManager::importData( std::istream& file, TriangleMesh& data ) {
    data = TriangleMesh();
    std::string line;
    while ( std::getline( file, line ) )
    {
        std::istringstream iss( line );
        std::string token;
        iss >> token;

        if ( token == "#" )
        {
            continue;
        }
        if ( token == "v" )
        {
            Vector3 v;
            iss >> v[0] >> v[1] >> v[2];
            data.vertices().push_back( v );
        }
        if ( token == "vn" )
        {
            Vector3 n;
            iss >> n[0] >> n[1] >> n[2];
            data.normals().push_back( n );
        }
        if ( token == "vt" )
        {
            continue;
        }
        if ( token == "vp" )
        {
            continue;
        }
        if ( token == "f" )
        {
            Triangle f;
            iss >> f[0] >> f[1] >> f[2];
            f[0] -= 1;
            f[1] -= 1;
            f[2] -= 1;
            data.m_triangles.push_back( f );
        }
    }
    if ( data.vertices().size() == 0 )
    {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    return true;
}

bool OBJFileManager::exportData( std::ostream& file, const TriangleMesh& data ) {
    std::string content = "";
    if ( data.vertices().size() == 0 )
    {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    // Vertices
    for ( uint i = 0; i < data.vertices().size(); ++i )
    {
        const Vector3 v = data.vertices().at( i );
        content += "v " + std::to_string( v[0] ) + " " + std::to_string( v[1] ) + " " +
                   std::to_string( v[2] ) + "\n";
    }
    // Normals
    for ( uint i = 0; i < data.normals().size(); ++i )
    {
        const Vector3 n = data.normals().at( i );
        content += "vn " + std::to_string( n[0] ) + " " + std::to_string( n[1] ) + " " +
                   std::to_string( n[2] ) + "\n";
    }
    // Triangle
    for ( uint i = 0; i < data.m_triangles.size(); ++i )
    {
        const Triangle f = data.m_triangles.at( i );
        content += "f " + std::to_string( f[0] + 1 ) + " " + std::to_string( f[1] + 1 ) + " " +
                   std::to_string( f[2] + 1 ) + "\n";
    }
    file << content;
    return true;
}

} // namespace Core
} // namespace Ra
