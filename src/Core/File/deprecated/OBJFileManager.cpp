#include <Core/File/deprecated/OBJFileManager.hpp>

namespace Ra {
namespace Core {

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
    data.clear();
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
            Vector3ui f;
            std::string ltoken;
            int count = 0;
            // skip first space
            std::getline( iss, ltoken, ' ' );
            while ( std::getline( iss, ltoken, ' ' ) )
            {
                std::istringstream ss( ltoken );
                if ( count < 3 )
                {
                    ss >> f[count];
                    f[count] -= 1;
                } else
                {
                    addLogErrorEntry( "MESH CONTAINS QUADS." );
                    return false;
                }
                count++;
            }
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

bool OBJFileManager::exportData( std::ostream& file, const Geometry::TriangleMesh& data ) {
    std::string content = "";
    if ( data.vertices().size() == 0 )
    {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    // Vertices
    for ( const auto& v : data.vertices() )
    {
        content += "v " + std::to_string( v[0] ) + " " + std::to_string( v[1] ) + " " +
                   std::to_string( v[2] ) + "\n";
    }
    // Normals
    for ( const auto& n : data.normals() )
    {
        content += "vn " + std::to_string( n[0] ) + " " + std::to_string( n[1] ) + " " +
                   std::to_string( n[2] ) + "\n";
    }
    // Triangle
    for ( const auto& f : data.m_triangles )
    {
        content += "f " + std::to_string( f[0] + 1 ) + " " + std::to_string( f[1] + 1 ) + " " +
                   std::to_string( f[2] + 1 ) + "\n";
    }
    file << content;
    return true;
}

} // namespace Core
} // namespace Ra
