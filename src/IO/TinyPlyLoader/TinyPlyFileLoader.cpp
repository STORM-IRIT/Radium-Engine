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
        LOG( logINFO ) << "....................................................................\n";
        for ( auto c : file.get_comments() )
            std::cout << "Comment: " << c;
        for ( auto e : file.get_elements() )
        {
            LOG( logINFO ) << "element - " << e.name << " (" << e.size << ")";
            for ( auto p : e.properties )
                std::cout << "\tproperty - " << p.name << " ("
                          << tinyply::PropertyTable[p.propertyType].str << ")";
        }
        LOG( logINFO ) << "....................................................................\n";
    }

    // The count returns the number of instances of the property group. The vectors
    // above will be resized into a multiple of the property group size as
    // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}

    auto initBuffer = [&file]( const std::string& elementKey,
                               const std::vector<std::string> propertyKeys ) {
        std::shared_ptr<tinyply::PlyData> ret;
        try
        { ret = file.request_properties_from_element( elementKey, propertyKeys ); }
        catch ( const std::exception& e )
        {
            ret = nullptr;
            LOG( logERROR ) << "[TinyPLY] " << e.what();
        }
        return ret;
    };

    auto vertBuffer {initBuffer( "vertex", {"x", "y", "z"} )};

    // if there is no vertex prop, or their count is 0, then quit.
    if ( !vertBuffer || vertBuffer->count == 0 )
    {
        delete fileData;
        LOG( logINFO ) << "[TinyPLY] No vertice found";
        return nullptr;
    }

    auto startTime {std::clock()};

    // a unique name is required by the component messaging system
    static int nameId {0};
    auto geometry = std::make_unique<GeometryData>( "PC_" + std::to_string( ++nameId ),
                                                    GeometryData::POINT_CLOUD );
    geometry->setFrame( Core::Transform::Identity() );

    auto normalBuffer {initBuffer( "vertex", {"nx", "ny", "nz"} )};
    auto alphaBuffer {initBuffer( "vertex", {"alpha"} )};
    auto colorBuffer {initBuffer( "vertex", {"red", "green", "blue"} )};

    // read buffer data from file content
    file.read( ss );

    auto copyBufferToGeometry = []( const std::shared_ptr<tinyply::PlyData>& buffer,
                                    Ra::Core::Vector3Array& container ) {
        if ( buffer && buffer->count != 0 )
        {
            auto floatBuffer = reinterpret_cast<float*>( buffer->buffer.get() );
            container.reserve( buffer->count );
            for ( size_t i = 0; i < buffer->count; ++i )
            {
                container.emplace_back(
                    floatBuffer[i * 3 + 0], floatBuffer[i * 3 + 1], floatBuffer[i * 3 + 2] );
            }
        }
    };

    copyBufferToGeometry( vertBuffer, geometry->getVertices() );
    copyBufferToGeometry( normalBuffer, geometry->getNormals() );

    size_t colorCount = colorBuffer ? colorBuffer->count : 0;
    if ( colorCount != 0 )
    {
        auto& container = geometry->getColors();
        container.reserve( colorCount );

        if ( alphaBuffer && alphaBuffer->count == colorCount )
        {
            uint8_t* al     = alphaBuffer->buffer.get();
            uint8_t* colors = colorBuffer->buffer.get();
            for ( size_t i = 0; i < colorCount; ++i, al++, colors += 3 )
            {
                container.emplace_back( Scalar( colors[0] ) / 255_ra,
                                        Scalar( colors[1] ) / 255_ra,
                                        Scalar( colors[2] ) / 255_ra,
                                        Scalar( *al ) / 255_ra );
            }
        }
        else
        {
            uint8_t* colors = colorBuffer->buffer.get();
            for ( size_t i = 0; i < colorCount; ++i, colors += 3 )
            {
                container.emplace_back( Scalar( colors[0] ) / 255_ra,
                                        Scalar( colors[1] ) / 255_ra,
                                        Scalar( colors[2] ) / 255_ra,
                                        1_ra );
            }
        }
    }

    fileData->m_geometryData.clear();
    fileData->m_geometryData.reserve( 1 );
    fileData->m_geometryData.push_back( std::move( geometry ) );

    fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

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
