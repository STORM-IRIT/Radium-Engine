#include <IO/TinyPlyLoader/TinyPlyFileLoader.hpp>

#include <Core/Asset/FileData.hpp>

#include <tinyply.h>

#include <fstream>
#include <iostream>
#include <string>

const std::string plyExt( "ply" );

namespace Ra {
namespace IO {

using namespace Core::Utils; // log
using namespace Core::Asset; // Filedata

TinyPlyFileLoader::TinyPlyFileLoader() {}

TinyPlyFileLoader::~TinyPlyFileLoader() {}

std::vector<std::string> TinyPlyFileLoader::getFileExtensions() const {
    return std::vector<std::string>( {"*." + plyExt} );
}

bool TinyPlyFileLoader::handleFileExtension( const std::string& extension ) const {
    return extension.compare( plyExt ) == 0;
}

FileData* TinyPlyFileLoader::loadFile( const std::string& filename ) {

    // Read the file and create a std::istringstream suitable
    // for the lib -- tinyply does not perform any file i/o.
    std::ifstream ss( filename, std::ios::binary );

    // Parse the ASCII header fields
    tinyply::PlyFile file;
    file.parse_header( ss );

    auto elements = file.get_elements();
    if ( std::any_of( elements.begin(), elements.end(), []( const auto& e ) -> bool {
             return e.name == "face" && e.size != 0;
         } ) )
    {
        // Mesh found. Let the other loaders handle it
        LOG( logINFO ) << "[TinyPLY] Faces found. Aborting" << std::endl;
        return nullptr;
    }

    // we are now sure to have a point-cloud
    FileData* fileData = new FileData( filename );

    if ( !fileData->isInitialized() )
    {
        delete fileData;
        LOG( logINFO ) << "[TinyPLY] Filedata cannot be initialized...";
        return nullptr;
    }

    if ( fileData->isVerbose() )
    {
        LOG( logINFO ) << "[TinyPLY] File Loading begin...";

        LOG( logINFO )
            << "........................................................................\n";
        for ( auto c : file.get_comments() )
            std::cout << "Comment: " << c;
        for ( auto e : file.get_elements() )
        {
            LOG( logINFO ) << "element - " << e.name << " (" << e.size << ")";
            for ( auto p : e.properties )
                std::cout << "\tproperty - " << p.name << " ("
                          << tinyply::PropertyTable[p.propertyType].str << ")";
        }
        LOG( logINFO )
            << "........................................................................\n";
    }

    // The count returns the number of instances of the property group. The vectors
    // above will be resized into a multiple of the property group size as
    // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
    std::shared_ptr<tinyply::PlyData> vertBuffer;
    try
    { vertBuffer = file.request_properties_from_element( "vertex", {"x", "y", "z"} ); }
    catch ( const std::exception& e )
    {
        vertBuffer = nullptr;
        LOG( logERROR ) << "[TinyPLY] " << e.what();
    }

    if ( vertBuffer && vertBuffer->count == 0 )
    {
        delete fileData;
        LOG( logINFO ) << "[TinyPLY] No vertice found";
        return nullptr;
    }

    fileData->m_geometryData.clear();
    fileData->m_geometryData.reserve( 1 );

    static int nameId = 0;
    // a unique name is required by the component messaging system
    auto geometry = std::make_unique<GeometryData>( "PC_" + std::to_string( ++nameId ),
                                                    GeometryData::POINT_CLOUD );
    geometry->setFrame( Core::Transform::Identity() );

    std::shared_ptr<tinyply::PlyData> normalBuffer( nullptr ), alphaBuffer( nullptr ),
        colorBuffer( nullptr );
    try
    { normalBuffer = file.request_properties_from_element( "vertex", {"nx", "ny", "nz"} ); }
    catch ( const std::exception& e )
    {
        normalBuffer = nullptr;
        LOG( logERROR ) << "[TinyPLY] " << e.what();
    }
    try
    { alphaBuffer = file.request_properties_from_element( "vertex", {"alpha"} ); }
    catch ( const std::exception& e )
    {
        alphaBuffer = nullptr;
        LOG( logERROR ) << "[TinyPLY] " << e.what();
    }
    try
    { colorBuffer = file.request_properties_from_element( "vertex", {"red", "green", "blue"} ); }
    catch ( const std::exception& e )
    {
        colorBuffer = nullptr;
        LOG( logERROR ) << "[TinyPLY] " << e.what();
    }

    std::clock_t startTime;
    startTime = std::clock();

    file.read( ss );

    {
        const size_t numVerticesBytes = vertBuffer->buffer.size_bytes();
        std::vector<Eigen::Matrix<float, 3, 1, Eigen::DontAlign>> verts( vertBuffer->count );
        std::memcpy( verts.data(), vertBuffer->buffer.get(), numVerticesBytes );
        geometry->setVertices( verts );
    }

    if ( normalBuffer && normalBuffer->count != 0 )
    {
        const size_t numVerticesBytes = normalBuffer->buffer.size_bytes();
        std::vector<Eigen::Matrix<float, 3, 1, Eigen::DontAlign>> normals( normalBuffer->count );
        std::memcpy( normals.data(), normalBuffer->buffer.get(), numVerticesBytes );
        geometry->setNormals( normals );
    }

    size_t colorCount = colorBuffer ? colorBuffer->count : 0;
    if ( colorCount != 0 )
    {
        auto& container = geometry->getColors();
        container.resize( colorCount );

        const size_t numVerticesBytes = colorBuffer->buffer.size_bytes();
        std::vector<Eigen::Matrix<uint8_t, 3, 1, Eigen::DontAlign>> colors( colorBuffer->count );
        std::memcpy( colors.data(), colorBuffer->buffer.get(), numVerticesBytes );

        auto* cols = colors.data();

        if ( alphaBuffer && alphaBuffer->count == colorCount )
        {
            uint8_t* al = alphaBuffer->buffer.get();
            for ( auto& c : container )
            {
                c = Core::Utils::Color::fromRGB( ( *cols ).cast<Scalar>() / 255_ra,
                                                 Scalar( *al ) / 255_ra );
                cols++;
                al++;
            }
        }
        else
        {
            for ( auto& c : container )
            {
                c = Core::Utils::Color::fromRGB( ( *cols ).cast<Scalar>() / 255_ra );
                cols++;
            }
        }
    }

    fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    fileData->m_geometryData.push_back( std::move( geometry ) );

    if ( fileData->isVerbose() )
    {
        LOG( logINFO ) << "[TinyPLY] File Loading end.";

        fileData->displayInfo();
    }

    fileData->m_processed = true;

    return fileData;
}

std::string TinyPlyFileLoader::name() const {
    return "TinyPly";
}
} // namespace IO
} // namespace Ra
