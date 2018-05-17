#include <Core/Asset/OBJFileManager.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
OBJFileManager::OBJFileManager() : FileManager<Geometry::TriangleMesh>() {}

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

bool OBJFileManager::importData( std::istream& file, Geometry::TriangleMesh& data ) {
    data = Geometry::TriangleMesh();
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
            Math::Vector3 v;
            iss >> v[0] >> v[1] >> v[2];
            data.m_vertices.push_back( v );
        }
        if ( token == "vn" )
        {
            Math::Vector3 n;
            iss >> n[0] >> n[1] >> n[2];
            data.m_normals.push_back( n );
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
            Geometry::Triangle f;
            iss >> f[0] >> f[1] >> f[2];
            f[0] -= 1;
            f[1] -= 1;
            f[2] -= 1;
            data.m_triangles.push_back( f );
        }
    }
    if ( data.m_vertices.size() == 0 )
    {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    return true;
}

bool OBJFileManager::exportData( std::ostream& file, const Geometry::TriangleMesh& data ) {
    std::string content = "";
    if ( data.m_vertices.size() == 0 )
    {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    // Vertices
    for ( uint i = 0; i < data.m_vertices.size(); ++i )
    {
        const Math::Vector3 v = data.m_vertices.at( i );
        content += "v " + std::to_string( v[0] ) + " " + std::to_string( v[1] ) + " " +
                   std::to_string( v[2] ) + "\n";
    }
    // Normals
    for ( uint i = 0; i < data.m_normals.size(); ++i )
    {
        const Math::Vector3 n = data.m_normals.at( i );
        content += "vn " + std::to_string( n[0] ) + " " + std::to_string( n[1] ) + " " +
                   std::to_string( n[2] ) + "\n";
    }
    // Triangle
    for ( uint i = 0; i < data.m_triangles.size(); ++i )
    {
        const Geometry::Triangle f = data.m_triangles.at( i );
        content += "f " + std::to_string( f[0] + 1 ) + " " + std::to_string( f[1] + 1 ) + " " +
                   std::to_string( f[2] + 1 ) + "\n";
    }
    file << content;
    return true;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
