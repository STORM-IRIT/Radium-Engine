#include <Core/Debug/Loading/FileData.hpp>

#include <utility>
#include <set>
#include <map>

#include <Core/Log/Log.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <Core/Debug/Loading/AssimpWrapper.hpp>

#include <Core/Debug/Loading/AssimpGeometryDataLoader.hpp>
#include <Core/Debug/Loading/AssimpHandleDataLoader.hpp>
#include <Core/Debug/Loading/AssimpAnimationDataLoader.hpp>

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

    AssimpGeometryDataLoader geometryLoader( m_verbose );
    geometryLoader.loadData( scene, m_geometryData );

    AssimpHandleDataLoader handleLoader( m_verbose );
    handleLoader.loadData( scene, m_handleData );

    AssimpAnimationDataLoader animationLoader( m_verbose );
    animationLoader.loadData( scene, m_animationData );

    if( m_verbose ) {
        LOG( logDEBUG ) << "File Loading end.";
    }

    m_processed = true;
}



} // namespace Asset
} // namespace Ra

