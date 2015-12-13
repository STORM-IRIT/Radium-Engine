#include <Engine/Assets/FileData.hpp>

#include <utility>
#include <set>
#include <map>
#include <ctime>

#include <Core/Log/Log.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <Engine/Assets/AssimpWrapper.hpp>
#include <Engine/Assets/AssimpGeometryDataLoader.hpp>
#include <Engine/Assets/AssimpHandleDataLoader.hpp>
#include <Engine/Assets/AssimpAnimationDataLoader.hpp>

namespace Ra {
namespace Asset {

/// LOAD
void FileData::loadFile( const bool FORCE_RELOAD ) {
    if( isProcessed() && FORCE_RELOAD ) {
        std::string filename = getFileName();
        reset();
        setFileName( filename );
    }

    if( !isInitialized() ) {
        return;
    }

    // File extension check
    // - If we decide to deal with user-defined file, here we should check if we are dealing with one of them or not

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile( getFileName(),
                                              aiProcess_Triangulate           | // This could/should be taken away if we want to deal with mesh types other than trimehses
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_GenSmoothNormals      |
                                              aiProcess_SortByPType           |
                                              aiProcess_FixInfacingNormals    |
                                              aiProcess_CalcTangentSpace      |
                                              aiProcess_GenUVCoords );

    // File was not loaded
    if( scene == nullptr ) {
        LOG( logERROR ) << "Error while loading file \"" << getFileName() << "\" : " << importer.GetErrorString() << ".";
        return;
    }

    if( m_verbose ) {
        LOG( logDEBUG ) << "File Loading begin...";
    }

    std::clock_t startTime;
    startTime = std::clock();

    AssimpGeometryDataLoader geometryLoader( Core::StringUtils::getDirName( getFileName() ), m_verbose );
    geometryLoader.loadData( scene, m_geometryData );

    // FIXME(Charly): Commented for debug purposes, uncomment it later
#ifdef DEBUG_LOAD_HANDLE
    AssimpHandleDataLoader handleLoader( m_verbose );
    handleLoader.loadData( scene, m_handleData );
#endif
#ifdef DEBUG_LOAD_ANIMATION
    AssimpAnimationDataLoader animationLoader( m_verbose );
    animationLoader.loadData( scene, m_animationData );
#endif

    m_loadingTime = std::clock() - startTime;

    if( m_verbose ) {
        LOG( logDEBUG ) << "File Loading end.";
        displayInfo();
    }

    m_processed = true;
}



void displayInfo() const {
    LOG( logDEBUG ) << "======== LOADING SUMMARY ========";
    LOG( logDEBUG ) << "Mesh loaded      : " << m_geometryData.size();
#ifdef DEBUG_LOAD_HANDLE
    LOG( logDEBUG ) << "Handle loaded    : " << m_handleData.size();
#endif
#ifdef DEBUG_LOAD_ANIMATION
    LOG( logDEBUG ) << "Animation loaded : " << m_animationData.size();
#endif
    LOG( logDEBUG ) << "Loading Time     : " << m_loadingTime;
}


} // namespace Asset
} // namespace Ra

