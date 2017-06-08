#include <Core/Utils/File/OBJFileManager.hpp>

namespace Ra {
namespace Core {

/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
OBJFileManager::OBJFileManager() : FileManager< TriangleMesh >() { }



/// ===============================================================================
/// DESTRUCTOR
/// ===============================================================================
OBJFileManager::~OBJFileManager() { }



/// ===============================================================================
/// INTERFACE
/// ===============================================================================
std::string OBJFileManager::fileExtension() const {
    return "obj";
}



bool OBJFileManager::importData( std::istream& file, TriangleMesh& data ) {
    data = TriangleMesh();
    std::string line;
    while( std::getline( file, line ) ) {
        std::istringstream iss( line );
        std::string token;
        iss >> token;

        if( token == "#" ) {
            continue;
        }
        if( token == "v" ) {
            Vector3 v;
            iss >> v[0] >> v[1] >> v[2];
            data.m_vertices.push_back( v );
        }
        if( token == "vn" ) {
            Vector3 n;
            iss >> n[0] >> n[1] >> n[2];
            data.m_normals.push_back( n );
        }
        if( token == "vt" ) {
            continue;
        }
        if( token == "vp" ) {
            continue;
        }
        if( token == "f" ) {
            Triangle f;
            iss >> f[0] >> f[1] >> f[2];
            f[0] -= 1;
            f[1] -= 1;
            f[2] -= 1;
            data.m_triangles.push_back( f );
        }
    }
    if( data.m_vertices.size() == 0 ) {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    return true;
}



bool OBJFileManager::exportData( std::ostream& file, const TriangleMesh& data ) {
    std::string content = "";
    if( data.m_vertices.size() == 0 ) {
        addLogErrorEntry( "MESH IS EMPTY." );
        return false;
    }
    // Vertices
    for( uint i = 0; i < data.m_vertices.size(); ++i ) {
        const Vector3 v = data.m_vertices.at( i );
        content += "v " + std::to_string( v[0] ) + " " +
                          std::to_string( v[1] ) + " " +
                          std::to_string( v[2] ) + "\n";
    }
    // Normals
    for( uint i = 0; i < data.m_normals.size(); ++i ) {
        const Vector3 n = data.m_normals.at( i );
        content += "vn " + std::to_string( n[0] ) + " " +
                           std::to_string( n[1] ) + " " +
                           std::to_string( n[2] ) + "\n";
    }
    // Face
    for( uint i = 0; i < data.m_faces.size(); ++i ) {
        const VectorNui f = data.m_faces.at( i );
        content += "f ";
        for (uint v = 0; v<f.size(); ++v)
        {
            content += std::to_string( f[v] + 1 ) + " ";
        }
        content += "\n";
    }
    file << content;
    return true;
}



} // namespace Core
} // namespace Ra
