#include <Core/Asset/FileData.hpp>
#include <IO/Gltf/Loader/glTFFileLoader.hpp>
#include <IO/Gltf/internal/GLTFConverter/Converter.hpp>

#include <IO/Gltf/internal/fx/gltf.h>

namespace Ra {
namespace IO {
namespace GLTF {
using namespace Ra::Core::Asset;
using namespace Ra::Core::Utils;

glTFFileLoader::glTFFileLoader() = default;

glTFFileLoader::~glTFFileLoader() = default;

std::vector<std::string> glTFFileLoader::getFileExtensions() const {
    return { "*.gltf", "*.glb" };
}

bool glTFFileLoader::handleFileExtension( const std::string& extension ) const {
    return ( extension == "gltf" ) || ( extension == "glb" );
}

FileData* glTFFileLoader::loadFile( const std::string& filename ) {
    auto fileData = new FileData( filename );
    fileData->setVerbose( true );

    if ( !fileData->isInitialized() ) {
        delete fileData;
        return nullptr;
    }

    std::clock_t startTime;
    startTime = std::clock();

    fileData->m_geometryData.clear();
    fileData->m_animationData.clear();

    // Load data
    fx::gltf::Document gltfFile;
    // Load at most 100 buffers in total, each as large as 80mb...
    // additionally, place a quota on the file size as well
    fx::gltf::ReadQuotas readQuotas {};
    readQuotas.MaxBufferCount      = 100;                // default: 8
    readQuotas.MaxBufferByteLength = 1500 * 1024 * 1024; // default: 32mb
    readQuotas.MaxFileSize = 1500 * 1024 * 1024; // default: 32mb (applies to binary .glb only)

    try {
        if ( filename.substr( filename.size() - 3 ) == "glb" ) {
            gltfFile = fx::gltf::LoadFromBinary( filename, readQuotas );
        }
        else { gltfFile = fx::gltf::LoadFromText( filename, readQuotas ); }
    }
    catch ( std::exception& e ) {
        LOG( logERROR ) << "Catched std::exception exception : " << e.what();
        delete fileData;
        return nullptr;
    }

    // get the basedir of the document
    std::string baseDir = filename.substr( 0, filename.rfind( '/' ) + 1 );
    if ( baseDir.empty() ) { baseDir = "./"; }

    // convert gltf scenegraph to Filedata ...
    Converter convertGlTF( fileData, baseDir );

    if ( !convertGlTF( gltfFile ) ) {
        LOG( logERROR ) << "Unable to convert gltf scene " << filename << ". Aborting";
        delete fileData;
        return nullptr;
    }

    fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if ( fileData->isVerbose() ) { fileData->displayInfo(); }

    fileData->m_processed = true;

    return fileData;
}

std::string glTFFileLoader::name() const {
    return { "glTF 2.0" };
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
