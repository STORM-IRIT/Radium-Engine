#include <IO/AssimpLoader/AssimpFileLoader.hpp>

#include <Core/File/FileData.hpp>
#include <Core/Utils/StringUtils.hpp>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <IO/AssimpLoader/AssimpAnimationDataLoader.hpp>
#include <IO/AssimpLoader/AssimpGeometryDataLoader.hpp>
#include <IO/AssimpLoader/AssimpHandleDataLoader.hpp>
#include <IO/AssimpLoader/AssimpLightDataLoader.hpp>

#include <iostream>

namespace Ra {
namespace IO {

using namespace Core::Utils; // log

AssimpFileLoader::AssimpFileLoader() = default;

AssimpFileLoader::~AssimpFileLoader() = default;

std::vector<std::string> AssimpFileLoader::getFileExtensions() const {
    std::string extensionsList;

    m_importer.GetExtensionList( extensionsList );

    // source: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
    std::istringstream iss( extensionsList );
    std::string ext;
    std::vector<std::string> extensions;
    while ( std::getline( iss, ext, ';' ) )
    {
        extensions.push_back( ext );
    }
    return extensions;
}

bool AssimpFileLoader::handleFileExtension( const std::string& extension ) const {
    return m_importer.IsExtensionSupported( extension );
}

Asset::FileData* AssimpFileLoader::loadFile( const std::string& filename ) {
    auto fileData = new Asset::FileData( filename );

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

    // FIXME : this warkaround is related to assimp issue #2260 Mesh created for a light only file
    // (collada) https://github.com/assimp/assimp/issues/2260 For the moment, only allow to load
    // failes with Light only. If one need to load file with only skeleton or animation, this must
    // be changed. Note that loading only skeletons or animations from a file is not allowed in
    // Radium For now, Skeleton must be loaded from a file with meshes
    if ( scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE )
    {
        LOG( logWARNING ) << " ai scene is incomplete, just try to load lights ..";
        AssimpLightDataLoader lightLoader( Core::Utils::getDirName( filename ),
                                           fileData->isVerbose() );
        lightLoader.loadData( scene, fileData->m_lightData );
    } else
    {
        AssimpGeometryDataLoader geometryLoader( Core::Utils::getDirName( filename ),
                                                 fileData->isVerbose() );
        geometryLoader.loadData( scene, fileData->m_geometryData );

        // check if that the scene contains at least one mesh
        // Note that currently, Assimp is ALWAYS creating faces, even when
        // loading point clouds
        // (see 3rdPartyLibraries/Assimp/code/PlyLoader.cpp:260)
        bool ok = std::any_of( fileData->m_geometryData.begin(), fileData->m_geometryData.end(),
                               []( const auto& geom ) -> bool { return geom->hasFaces(); } );
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

        AssimpLightDataLoader lightLoader( Core::Utils::getDirName( filename ),
                                           fileData->isVerbose() );
        lightLoader.loadData( scene, fileData->m_lightData );
    }

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
