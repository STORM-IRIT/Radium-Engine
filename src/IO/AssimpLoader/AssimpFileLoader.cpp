#include <IO/AssimpLoader/AssimpFileLoader.hpp>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <IO/AssimpLoader/AssimpAnimationDataLoader.hpp>
#include <IO/AssimpLoader/AssimpGeometryDataLoader.hpp>
#include <IO/AssimpLoader/AssimpHandleDataLoader.hpp>
#include <IO/AssimpLoader/AssimpLightDataLoader.hpp>

#include <iostream>

namespace Ra {
namespace IO {

AssimpFileLoader::AssimpFileLoader() {}

AssimpFileLoader::~AssimpFileLoader() {}

std::vector<std::string> AssimpFileLoader::getFileExtensions() const {
    std::string extensionsList;

    m_importer.GetExtensionList( extensionsList );

    std::vector<std::string> extensions = Core::StringUtils::splitString( extensionsList, ';' );

    return extensions;
}

bool AssimpFileLoader::handleFileExtension( const std::string& extension ) const {
    return m_importer.IsExtensionSupported( extension );
}

Asset::FileData* AssimpFileLoader::loadFile( const std::string& filename ) {
    Asset::FileData* fileData = new Asset::FileData( filename );

    if ( !fileData->isInitialized() )
    {
        return nullptr;
    }

    const aiScene* scene = m_importer.ReadFile(
        fileData->getFileName(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                     aiProcess_SortByPType | aiProcess_FixInfacingNormals |
                                     aiProcess_CalcTangentSpace | aiProcess_GenUVCoords );

    if ( scene == nullptr )
    {
        LOG( logINFO ) << "File \"" << fileData->getFileName()
                       << "\" assimp error : " << m_importer.GetErrorString() << ".";
        return nullptr;
    }

    if ( fileData->isVerbose() )
    {
        LOG( logINFO ) << "File Loading begin...";
    }

    std::clock_t startTime;
    startTime = std::clock();

    AssimpGeometryDataLoader geometryLoader( Core::StringUtils::getDirName( filename ),
                                             fileData->isVerbose() );
    geometryLoader.loadData( scene, fileData->m_geometryData );

    // check if that the scene contains at least one mesh
    // Note that currently, Assimp is ALWAYS creating faces, even when
    // loading point clouds
    // (see 3rdPartyLibraries/Assimp/code/PlyLoader.cpp:260)
    bool ok = false;
    for ( const auto& geom : fileData->m_geometryData )
    {
        if ( geom->hasFaces() )
        {
            ok = true;
            break;
        }
    }
    if ( !ok )
    {
        if ( fileData->isVerbose() )
        {
            LOG( logINFO ) << "Point-cloud found. Aborting";
            delete fileData;
            return nullptr;
        }
    }

    AssimpHandleDataLoader handleLoader( fileData->isVerbose() );
    handleLoader.loadData( scene, fileData->m_handleData );

    AssimpAnimationDataLoader animationLoader( fileData->isVerbose() );
    animationLoader.loadData( scene, fileData->m_animationData );

    AssimpLightDataLoader lightLoader( Core::StringUtils::getDirName( filename ),
                                       fileData->isVerbose() );
    lightLoader.loadData( scene, fileData->m_lightData );

    fileData->m_loadingTime = ( std::clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    if ( fileData->isVerbose() )
    {
        LOG( logINFO ) << "File Loading end.";

        fileData->displayInfo();
    }

    fileData->m_processed = true;

    return fileData;
}

std::string AssimpFileLoader::name() const {
    return "Assimp";
}
} // namespace IO
} // namespace Ra
